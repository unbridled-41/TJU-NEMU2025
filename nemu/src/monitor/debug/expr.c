#include "nemu.h"
#include <elf.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
bool check_parentheses_correct(int p, int q);
bool check_parentheses_correct(int p, int q);
int max(int p, int q);
int how_long_reg[32], num_reg = 0;
int how_long_hex[32], num_hex = 0;
int how_long_x[32], num_x = 0;
extern char* strtab;
extern Elf32_Sym* symtab;
extern int nr_symtab_entry;
void tokens_str_clear(int i);
enum {
	NOTYPE = 256,
	NUMBER = 6,
	EQ = 2,
	ZUO = 3,
	YOU = 4,
	REG = 5,
	HEX = 1,
	NOTEQ = 7,
	AND = 8,
	OR = 9,
	NOT = 10,
	FU = 11,
	JIE = 12,
	OBJECT = 13,
	/* TODO: Add more token types */

};

static struct rule {
	char* regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"\\-", '-'},
	{"\\*", '*'},
	{"\\/", '/'},

	{"==", EQ},                      // equal
	{"!=", NOTEQ},

	{"\\(", ZUO},
	{"\\)", YOU},

	
	{"\\$(e?(ax|dx|cx|bx|si|di|sp|bp|ip)|[a-d][hl])", REG},
	{"0[xX][0-9a-fA-F]+", HEX},
	{"[0-9]+", NUMBER},
	{"[a-zA-Z_][a-zA-Z0-9_]*", OBJECT},
	{"&&", AND},
	{"[\\|+]", OR},
	{"!", NOT},
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

	for (i = 0; i < NR_REGEX; i++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32] = {};
int nr_token = 0;

static bool make_token(char* e) {
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;

	while (e[position] != '\0') {
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++) {
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				//char* substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				if (nr_token > 31) {
					printf("Toooooo long!");
					return false;
				}
				switch (rules[i].token_type) {
				case NOTYPE:
					break;
				case '+': {
					Token temp_token;
					temp_token.type = '+';
					tokens[nr_token++] = temp_token;
					break;
				}
				case '-': {
					Token temp_token;
					temp_token.type = '-';
					tokens[nr_token++] = temp_token;
					break;
				}
				case '*': {
					Token temp_token;
					temp_token.type = '*';
					tokens[nr_token++] = temp_token;
					break;
				}
				case '/': {
					Token temp_token;
					temp_token.type = '/';
					tokens[nr_token++] = temp_token;
					break;
				}
				case EQ: {
					Token temp_token;
					temp_token.type = EQ;
					tokens[nr_token++] = temp_token;
					break;
				}
				case ZUO: {
					Token temp_token;
					temp_token.type = ZUO;
					tokens[nr_token++] = temp_token;
					break;
				}
				case YOU: {
					Token temp_token;
					temp_token.type = YOU;
					tokens[nr_token++] = temp_token;
					break;
				}
				case NOT: {
					Token temp_token;
					temp_token.type = NOT;
					tokens[nr_token++] = temp_token;
					break;
				}
				case AND: {
					Token temp_token;
					temp_token.type = AND;
					tokens[nr_token++] = temp_token;
					break;
				}
				case OR: {
					Token temp_token;
					temp_token.type = OR;
					tokens[nr_token++] = temp_token;
					break;
				}
				case NOTEQ: {
					Token temp_token;
					temp_token.type = NOTEQ;
					tokens[nr_token++] = temp_token;
					break;
				}
				case NUMBER: {
					Token temp_token;
					int j = 0;
					for (; j < 32; j++) {
						temp_token.str[j] = '\0';
					}
					temp_token.type = NUMBER;
					if (substr_len <= 32) {
						tokens_str_clear(nr_token);
						strncpy(temp_token.str, &e[position - substr_len], substr_len);
						//printf("temp__token%s\n", temp_token.str);
					}
					else
						panic("the number is toooooooo long");
					tokens[nr_token++] = temp_token;
					break;
				}
				case REG:
					if (substr_len > 32)
						panic("the number is toooooooo long");
					tokens[nr_token].type = REG;
					tokens_str_clear(nr_token);
					strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len);
					nr_token++;
					how_long_reg[num_reg++] = substr_len;
					break;
				case HEX:
					if (substr_len > 32)
						panic("the number is toooooooo long");
					tokens[nr_token].type = HEX;
					tokens_str_clear(nr_token);
					strncpy(tokens[nr_token++].str, &e[position - substr_len], substr_len);
					how_long_hex[num_hex++] = substr_len;
					break;
				case OBJECT:
					tokens[nr_token].type = OBJECT;
					tokens_str_clear(nr_token);
					strncpy(tokens[nr_token++].str, &e[position - substr_len], substr_len);
					how_long_x[num_x++] = substr_len;
					break;
				default: panic("illegal char!");
				}
				break;
			}
		}

		if (i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}
bool check_parenthess(int p, int q) {
	int stack = 1;
	if (tokens[p].type != ZUO || tokens[q].type != YOU)
		return false;
	int i = 0;
	for (i = p + 1; i < q; i++) {
		if (tokens[i].type == ZUO) {
			stack++;
		}
		else if (tokens[i].type == YOU) {
			stack--;
		}
		if (stack == 0)
			return false;
	}
	return true;
}
bool check_parentheses_correct(int p, int q) { 
	int stack = 0;
	int i = 0;
	for (i = p; i <= q; i++) {
		if (tokens[i].type == ZUO) {
			stack++;
		}
		else if (tokens[i].type == YOU) {
			if (stack == 0) {
				return false;
			}
			stack--;
		}
	}
	return (stack == 0);
}
int max(int p, int q) {
	if (p > q)
		return p;
	else
		return q;
}
long long eval(int p, int q) {
	//printf("p = %d, q = %d\n", p, q);
	if (p > q) {
		panic("jisuan error!");
		return -1;
	}
	else if (p == q) {
		long long rt = 0;
		//printf("zhuan hua qian %s\n", tokens[p].str);
		sscanf(tokens[p].str, "%lld", &rt);
		//printf("hou %u\n", rt);
		return rt;
	}
	else if (check_parenthess(p, q) == true) {
		return eval(p + 1, q - 1);
	}
	else {
		int op = -1; 
		int i = 0;
		int pan = -3;
		int qaq = 0;
		for (i = p; i <= q; i++)
		{
			if (tokens[i].type == ZUO)
			{
				qaq += 1;
				while (qaq > 0) {
					i++;
					if (tokens[i].type == YOU) {
						qaq -= 1;
					}
					if (tokens[i].type == ZUO) {
						qaq += 1;
					}
				}
			}
			if ((pan <= 4) && (tokens[i].type == OR)) {
				pan = 4;
				op = max(op, i);
			}
			if ((pan <= 3) && (tokens[i].type == AND)) {
				pan = 3;
				op = max(op, i);
			}
			if ((pan <= 2) && ((tokens[i].type == EQ) || (tokens[i].type == NOTEQ))) {
				pan = 2;
				op = max(op, i);
			}
			if ((pan <= 1) && (tokens[i].type == '+' || tokens[i].type == '-')) {
				pan = 1;
				op = max(op, i);
			}
			if ((pan <= 0) && (tokens[i].type == '*' || tokens[i].type == '/')) {
				pan = 0;
				op = max(op, i);
			}
			if ((pan < -1) && (tokens[i].type == FU || tokens[i].type == NOT || tokens[i].type == JIE)) {
				pan = -1;
				op = max(op, i);
			}
		}
		//printf(" op = %d\n", op);
		int op_type = tokens[op].type;
		if (op_type == FU) {
			return -eval(p + 1, q);
		}
		if (op_type == NOT) {
			return !eval(p + 1, q);
		}
		if (op_type == JIE) {
			swaddr_t addr = 0;
			addr = eval(p + 1, q);
			return swaddr_read(addr, 4, R_DS);
		}
		//printf("chuanru p = %d, q = %d\n", p, op-1);
		long long  vaL1 = eval(p, op - 1);
		//printf("chuanru p = %d, q = %d\n", op+1, q);
		long long  vaL2 = eval(op + 1, q);

		//printf("%u %u", vaL1, vaL2);
		switch (op_type) {
		case '+':
			//printf("jisuan %u + %u\n", vaL1, vaL2);
			return vaL1 + vaL2;
		case '-':
			//printf("jisuan %u - %u\n", vaL1, vaL2);
			return vaL1 - vaL2;
		case '*':
			//printf("jisuan %u * %u\n", vaL1, vaL2);
			return vaL1 * vaL2;
		case '/':
			//printf("jisuan %u / %u\n", vaL1, vaL2);
			if (vaL2 == 0) {//printf("division can't zero;\n");
				panic("division can't zero;\n");
				return 0;
			}
			return vaL1 / vaL2;
		case EQ:
			return vaL1 == vaL2;
		case NOTEQ:
			return vaL1 != vaL2;
		case AND:
			return vaL1 && vaL2;
		case OR:
			return vaL1 || vaL2;
		default:
			printf("No Op type.");
			assert(0);
		}
	}
}
long long expr(char* e, bool* success) {
	//printf("dy %s\n", e);
	if (!make_token(e)) {
		*success = false;
		return 0;
	}
	int i = 0;
	for (i = 0; i < nr_token; i++)
	{
		if (tokens[i].type == OR) {
			int j = i + 1;
			nr_token--;
			for (; j < nr_token; j++)
				tokens[j] = tokens[j + 1];
		}
	}
	num_reg = 0;
	for (i = 0; i < nr_token; i++) {
		if (tokens[i].type == REG) {
			bool flag = false;
			tokens[i].type = NUMBER;
			char temp_str[32];
			strncpy(temp_str, &tokens[i].str[1], how_long_reg[num_reg]);
			int k = 0;
			for (; k < how_long_reg[num_reg]; k++) {
				if (temp_str[k] >= 'A' && temp_str[k] <= 'Z')
					temp_str[k] += 32;
			}
			temp_str[how_long_reg[num_reg++]] = '\0';
			int j = 0;
			for (j = 0; j < 8; j++) { //�Ĵ�����8��
				if (strcmp(temp_str, regsl[j]) == 0) {
					sprintf(tokens[i].str, "%d", reg_l(j));
					flag = true;
				}
			}
			for (j = 0; j < 8; j++) { //�Ĵ�����8��
				if (strcmp(temp_str, regsw[j]) == 0) {
					sprintf(tokens[i].str, "%d", reg_w(j));
					flag = true;
				}
			}
			for (j = 0; j < 8; j++) { //�Ĵ�����8��
				if (strcmp(temp_str, regsb[j]) == 0) {
					sprintf(tokens[i].str, "%d", reg_b(j));
					flag = true;
				}
			}
			if (strcmp(temp_str, "eip") == 0) {
				sprintf(tokens[i].str, "%d", cpu.eip);
				flag = true;
			}
			if (!flag) {
				panic("Register access failed!");
			}
		}
	}
	num_hex = 0;
	for (i = 0; i < nr_token; i++) {
		if (tokens[i].type == HEX) {
			char temp_str[32];
			strncpy(temp_str, &tokens[i].str[2], how_long_hex[num_hex] - 2);
			temp_str[how_long_hex[num_hex++] - 2] = '\0';
			long long num = 0;
			num = strtoull(temp_str, NULL, 16);
			tokens[i].type = NUMBER;
			sprintf(tokens[i].str, "%lld", num);
		}
	}
	num_x = 0;
	for (i = 0; i < nr_token; i++) {
		if (tokens[i].type == OBJECT) {
			char temp_qaq[32];
			strncpy(temp_qaq, &tokens[i].str[0], how_long_x[num_x]);
			temp_qaq[how_long_x[num_x++]] = '\0';
			//printf("%s\n", temp_qaq);
			int qwq = 0;
			bool pipei = false;
			for (; qwq < nr_symtab_entry; qwq++) {
				//printf("qwq =	%d\n",qwq);
				//bool flag = (symtab[qwq].st_info & 0xf) == STT_OBJECT;
				if ((symtab[qwq].st_info & 0xf) == STT_OBJECT){
					//printf("OBJ\n");
					char *tmp;
					//int tmplen = symtab[qwq + 1].st_name - symtab[qwq].st_name - 1;
					/*if (qwq == 9 || qwq == 8) {
						printf("wei = %d\n", symtab[qwq + 1].st_name);
						printf("shou = %d\n", symtab[qwq].st_name);
					}*/
					//printf("len = %d\n", tmplen);
					tmp = strtab + symtab[qwq].st_name;
					//strncpy(tmp, strtab + symtab[qwq].st_name, tmplen);
					//tmp[tmplen] = '\0';
					//printf("%s	%d\n", tmp,qwq);
					if (strcmp(temp_qaq, tmp) == 0) {
						pipei = true;
						tokens[i].type = NUMBER;
						long long object_ = symtab[qwq].st_value;
						sprintf(tokens[i].str, "%lld", object_);
						break;
					}
				}
			}
			if (!pipei) {
				printf("wrong name!");
				return 0;
			}
		}
	}
	for (i = 0; i < nr_token; i++) {
		if ((tokens[i].type == '-' && i > 0 && tokens[i - 1].type != NUMBER && tokens[i - 1].type != YOU) || (tokens[i].type == '-' && i == 0)) {
			tokens[i].type = FU;
		}
	}
	for (i = 0; i < nr_token; i++) {
		if (tokens[i].type == '*' && ((i == 0) || ((tokens[i - 1].type != NUMBER) && (tokens[i - 1].type != YOU)))) {
			tokens[i].type = JIE;
		}
	}
	if (!check_parentheses_correct(0, nr_token - 1)) {
		*success = false;
		panic("uncorrect parentheses");
	}
	//printf("eval(%d,%d)", 0, nr_token - 1);
	long long ceshi = eval(0, nr_token - 1);
	if (ceshi < 0) {
		panic("the result should > 0 or the answer is wrong");
	}
	else {
		*success = true;
		return ceshi;
	}
	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	//return 0;
}
void tokens_str_clear(int i) {
	int j = 0;
	for (; j < 32; j++) {
		tokens[i].str[j] = '\0';
	}
}
