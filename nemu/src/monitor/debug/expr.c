#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ = 4 ,
    NUM = 1,
	RESGISTER = 2,
    HEX = 3,
    NOTEQ = 5,
    OR = 6,
    AND = 7,
    POINT,NEG
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
	{"\\+", '+'}, 	// plus
	{"\\-",'-'},
	{"\\*",'*'},
	{"\\/",'/'},

	{"\\$[a-z]+", RESGISTER},
	{"0[xX][0-9a-fA-F]+", HEX},
	{"[0-9]+", NUM},
	{"==", EQ},	// equal

	{"!=", NOTEQ},

	{"\\(", '('},
	{"\\)", ')'},

	{"\\|\\|", OR},
	{"&&", AND},
	{"!", '!'},
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

Token tokens[32];
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
                                int j;
				 for (j = 0;j < 32;j++)
				 {
					 tokens[nr_token].str[j] = '\0';
				 }

		switch(rules[i].token_type) {
			case 256:
			break;
			case 1:
			tokens[nr_token].type = 1;
	strncpy(tokens[nr_token].str,&e[position - substr_len],substr_len);
			nr_token++;
			break;

			case 2:
                        tokens[nr_token].type = 2;
        strncpy(tokens[nr_token].str,&e[position - substr_len],substr_len);
                        nr_token++;
                        break;

			case 3:
                        tokens[nr_token].type = 3;
        strncpy(tokens[nr_token].str,&e[position - substr_len],substr_len);
                        nr_token++;
                        break;

			case 4:
                        tokens[nr_token].type = 4;
                        strcpy(tokens[nr_token].str,"==");
			nr_token++;
			break;

			case 5:
                        tokens[nr_token].type = 5;
                        strcpy(tokens[nr_token].str,"!=");
                        nr_token++;
                        break;

			case 6:
                        tokens[nr_token].type = 6;
                        strcpy(tokens[nr_token].str,"||");
                        nr_token++;
                        break;

			case 7:
                        tokens[nr_token].type = 7;
                        strcpy(tokens[nr_token].str,"&&");
                        nr_token++;
                        break;

			case '+':
                        tokens[nr_token].type = '+';
                        nr_token++;
                        break;

			case '-':
                        tokens[nr_token].type = '-';
                        nr_token++;
                        break;

			case '*':
                        tokens[nr_token].type = '*';
                        nr_token++;
                        break;

			case '/':
                        tokens[nr_token].type = '/';
                        nr_token++;
                        break;

			case '!':
                        tokens[nr_token].type = '!';
                        nr_token++;
                        break;

			case '(':
                        tokens[nr_token].type = '(';
                        nr_token++;
                        break;

			case ')':
                        tokens[nr_token].type = ')';
                        nr_token++;
                        break;
					default: panic("please implement me");
					 assert(0);
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	panic("please implement me");
	return 0;
}

bool check_parentheses(int p,int q){
	int a;
	int j = 0,k = 0;
	if (tokens[p].type == '('||tokens[q].type == ')'){
		for(a = p;a <= q;a++)
			if (tokens[a].type == '('){
				j++;
			}
                        if (tokens[a].type == ')'){
                                k++;
                        }

			if (a != q && j == k){
	
				return false;

			}

			if (j == k){
	
				return true;

			} else {
	
				return false;
                        }
		}
	return false;
}

int dominant_operator(int p,int q){
	int step = 0;
	int i;
	int op = -1;
	int pri = 0;

	for (i = p;i <= q;i++){
		if(tokens[i].type == '('){
			step++;
		}
	        if(tokens[i].type == ')'){
                        step--;
                }

		if (step == 0){
			if (tokens[i].type == OR){
				if (pri < 51){
					op = i;
					pri = 51;
				}
			}else if(tokens[i].type == AND){
				if (pri < 50){
                                        op = i;
                                        pri = 50;
                                }
			}else if(tokens[i].type == EQ||tokens[i].type == NOTEQ){                               if (pri < 49){ 
				op = i;
				pri = 49;
			}
			}else if(tokens[i].type == '+'||tokens[i].type == '-'){
				if (pri < 48){
                                        op = i;
                                        pri = 48;
                                }
			}else if(tokens[i].type == '*'||tokens[i].type == '/'){
                                if (pri < 46){
                                        op = i;
                                        pri = 46;
                                }
			}
			else if (step < 0){
				return -2;
			}
		}
	}
	return op;
}