#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

swaddr_t find_elf_object(char *name);

enum {
	NOTYPE = 256, EQ, NEQ, LEQ, GEQ,
	AND, OR,
	NEG, REF,
	DECNUM, HEXNUM,
	REGNAME, OBJECT,
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"\\-", '-'},                                   // minus
	{"\\*", '*'},					// multiplication
	{"\\/", '/'},					// division
	{"\\%", '%'},					// modulo
	{"\\(", '('},					// left backet
	{"\\)", ')'},					// right backet

	{"==", EQ},						// equal	
	{"<=", LEQ},
	{">=", GEQ},
	{">", '>'},
	{"<", '<'},
	{"!=", NEQ},
	{"\\=", '='},
	{"\\!",'!'},
	{"\\&\\&",AND},
	{"\\|\\|",OR},
	{"0x[0-9a-fA-F]{1,10}", HEXNUM},
	{"[0-9]{1,10}", DECNUM},
	{"\\$[a-z]{2,3}", REGNAME},
	{"[a-zA-Z_][a-zA-Z0-9_]+", OBJECT},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[12800];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				if(rules[i].token_type == NOTYPE)
					break;
				int ret;
				switch(rules[i].token_type) {
					case DECNUM:
					case HEXNUM:
					case OBJECT:
						ret = sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
						Assert(ret == substr_len, "Numeric constant too large.");
						break;					
					case REGNAME:
						sprintf(tokens[nr_token].str, "%.*s", substr_len - 1, substr_start + 1);
						break;	
					case '+':
					case '-':
					case '*':
					case '/':
					case '%':
					case '(':
					case ')':
					case '=':
					case '<':
					case '>':
					case '!':
					case AND:
					case OR:
					case NEG:
					case REF:
					case EQ:
					case NEQ:
					case LEQ:
					case GEQ:
						break;
					default: 
						panic("please implement me");
				}
				tokens[nr_token].type = rules[i].token_type;
				nr_token++;

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
		if(nr_token > 12800){
			printf("The expression is too long:\n %s.\n",e);
			return false;
}
	}

	return true; 
}
static bool check_parentheses(int p,int q){
	if(p > q)return 0;
	int i,left_num=0;
	for(i = p;i<=q;i++){
		if(tokens[i].type=='(')
			left_num++;
		else if(tokens[i].type==')'){
			if(left_num > 0)
				left_num--;
			else 
				return false;
		}
	}
	return left_num==0;
}
static int computing_priority(int operator){
	switch(operator){
		case '+':
		case '-':
			return 4;
		case '*':
		case '/':
		case '%':
			return 3;
		case '=':
			return 14;
		case '<':
		case '>':
		case LEQ:
		case GEQ:
			return 6;
		case EQ:
		case NEQ:
			return 7;

		case AND:
			return 11;
		case OR:
			return 12;
		case '!':
		case NEG:
		case REF:
			return 2;
		case '(':
			return 1;
		case ')':
			return 0;
		default:
			return -1;
	}return -1;

}
static uint32_t do_operate(uint32_t a, uint32_t b, int operator){
	switch(operator){
		case '+':
			return a + b;
		case '-':
			return a - b;
		case '*':
			return a * b;
		case '/':
			return a / b;
		case '%':
			return a % b;
		case '=':
			return a = b;
		case '<':
			return a < b;
		case '>':
			return a > b;
		case AND:
			return a && b;
		case OR:
			return a || b;
		case EQ:
			return a == b;
		case NEQ:
			return a != b;
		case LEQ:
			return a <= b;
		case GEQ:
			return a >= b;
		default:
			return -1;
	}return -1;

}
static uint32_t eval(int p,int q,bool *success){
	uint32_t val;
	if(p > q){
		panic("Bad Expression");
		*success = false;	
	}else if(p == q){
		switch(tokens[p].type){
			case DECNUM:
				sscanf(tokens[p].str,"%u",&val);
				return val;
			case HEXNUM:
				sscanf(tokens[p].str,"%x",&val);
				return val;
			case REGNAME:
				if(!strcmp(tokens[p].str,"eip"))
					return cpu.eip;

				int i;
				for(i = 0;i < 8;i++){
					if(!strcmp(regsl[i],tokens[p].str))
						return reg_l(i);
					if(!strcmp(regsw[i],tokens[p].str))
						return reg_w(i);
					if(!strcmp(regsb[i],tokens[p].str))
						return reg_b(i);
				}
				break;	
			case OBJECT:
				val = find_elf_object(tokens[p].str);
				Assert(val!=0, "The object don't exist!");
				return val;
				break;
			default:
				break;
		}		panic("Bad Expression");
				*success = false; 

	}
	else if(tokens[p].type=='('&&tokens[q].type==')'&&check_parentheses(p + 1,q - 1)){
		return eval(p + 1,q - 1,success);
	}
	else{
		int i;
		int left_num = 0;
		int dom_op = 0,dom_op_priority = -1;
		for(i = p;i <= q; i++){
			if(tokens[i].type == '(')
				left_num++;
			else if(tokens[i].type == ')')
				left_num--;
			else if(!left_num){
				int priority=computing_priority(tokens[i].type);		
				if(priority>=dom_op_priority){
					dom_op=i;
					dom_op_priority = priority;
				}
			}
		}if(dom_op_priority == 2){
			val = eval(p+1 , q,success);
			if(*success){
				switch(tokens[p].type){
					case NEG:
						return -val;
					case REF:
						return swaddr_read(val,4);
					case '!':
						return !val;
					default:
						break;
				}
			}
			panic("Bad Expression");
			*success = false; 

		}else{

			uint32_t lval = eval(p,dom_op-1,success);
			if(*success == false){return -1;}
			uint32_t rval = eval(dom_op + 1, q, success);
			if(*success == false){return -1;}
			Assert(!(tokens[dom_op].type == '/' && rval == 0), "divide by 0!");
			val = do_operate(lval, rval, tokens[dom_op].type);
			return val;
		}
	}
	return -1;
}
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	if(check_parentheses(0,nr_token-1)){
		int i;
		for(i = 0;i < nr_token;i ++){
			// recognize the unary operator
			if(tokens[i].type == '-'&&(!i||computing_priority(tokens[i-1].type)>0))
				tokens[i].type = NEG;
			else if(tokens[i].type == '*'&&(!i||computing_priority(tokens[i-1].type)>0))
				tokens[i].type = REF;
		}
		uint32_t val = eval(0, nr_token-1, success);
		if(*success){return val;}	
	}
	*success = false;
	panic("please implement me");
	return 0;
}
