#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);


/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char* args) {
	int step = 0, result = 0;
	if (args == NULL) {
		step = 1;
	}
	else if (*args != '-') {
		while (*args && (*args != ' ')) {
			result = result * 10 + (*args++ - '0');
		}
		step = result;
	}
	else if (*args == '-')
		panic("the number should be > 0!");
	uint32_t N = step;
	while (N > 0){
		N--;
		cpu_exec(1);
	}
	return 0;
}

static int cmd_info(char* args) {
	switch (*args) {
	case 'r': {
		int length1 = sizeof(cpu.gpr) / sizeof(cpu.gpr[0]);
		int i = 0;
		for (i = 0; length1 > i; i++) {
			printf("%s				0x%08x				%d\n", regsl[i], (unsigned int)reg_l(i), reg_l(i));
		}
		printf("eip				0x%08x				%d\n", cpu.eip, cpu.eip);
		printf("|GDTR:\tbase: 0x%x\tlimit: 0x%x\t\t|\t\t", cpu.GDTR.base, cpu.GDTR.limit);
		printf("page dictionary:\tbase: 0x%x|\n", cpu.cr3.page_directory_base);
		printf("selector:\tCS: %d\tDS: %d\tSS: %d\tES: %d\n", cpu.CS.selector, cpu.DS.selector, cpu.SS.selector, cpu.ES.selector);
		printf("base:\t\tCS: %d\tDS: %d\tSS: %d\tES: %d\n", cpu.CS.base, cpu.DS.base, cpu.SS.base, cpu.ES.base);
		break;
	}
	case 'w':
		printf("YES\n");
		display_wp();
		break;
	default:
		break;
	}
	return 0;
}
static int cmd_x(char* args) {
	char* number1 = strtok(args, " ");
	char* number2 = number1 + strlen(number1) + 1;
	int n = 0;
	swaddr_t addr = 0;
	sscanf(number1, "%d", &n);
	if(*number2 == '0' &&(number2[1] == 'x' || number2[1] == 'X')){
		sscanf(number2 + 2, "%x", &addr);
	}
	int count = 0;
	while (n--) {
		if (count % 4 == 0) {
			printf("0x%08x: ", addr);
		}
		printf("0x%08x ", swaddr_read(addr, 4, R_DS));
		addr += 4;
		count++;
		if (count > 0 && (count % 4 == 0))
			printf("\n");
		
	}
	printf("\n");
	return 0;
	
}

static int cmd_p(char* args) {
	if (args == NULL) {
		printf("please input Expression");
		return 0;
	}
	bool flag = false;
	uint32_t jieguo = 0;
	jieguo	= expr(args, &flag);
	printf("0x%08x(%u)\n", jieguo, jieguo);
	return 0;
}
static int cmd_w(char* args) {
	create_wp(args);
	return 0;
}
static int cmd_d(char* args) {
	if (args == NULL)
		printf("please input the number of watchpoint");
	else {
		int num = 0;
		sscanf(args, "%d", &num);
		delete_wp(num);
	}
	return 0;
}
typedef struct {
	swaddr_t prev_ebp;  //%ebp的旧值
	swaddr_t ret_addr;   //返回地址
	uint32_t args[4];     //函数的实参
} PartOfStackFrame;
static int cmd_bt(char* args) {
	int i = 0;
	PartOfStackFrame now;		
	int ebp = reg_l(R_EBP);		
	now.ret_addr = cpu.eip;
	while (ebp) {
		int j = 0;
		for (j = 0; j < nr_symtab_entry; j++) {	
			if ((symtab[j].st_info & 0xf) == STT_FUNC) {
				if (symtab[j].st_value <= now.ret_addr && now.ret_addr < symtab[j].st_value + symtab[j].st_size) {
					printf("#%d\treturn 0x%08x in %s", i++, swaddr_read(ebp + 4, 4, R_SS), strtab + symtab[j].st_name); 
					break;
				}
			}
		}
		now.prev_ebp = swaddr_read(ebp, 4, R_SS);
		now.ret_addr = swaddr_read(ebp + 4, 4, R_SS);
		int k = 0;	for (k = 0; k < 4; k++) now.args[k] = swaddr_read(ebp + 8 + 4 * k, 4, R_SS);
		printf("(%d, %d, %d, %d)\n", now.args[0], now.args[1], now.args[2], now.args[3]);
		ebp = now.prev_ebp;	
	}
	return 0;
}

static int cmd_help(char *args);

hwaddr_t cmd_page(lnaddr_t addr);
static int cmd_translate(char* args) {
	if (args == NULL) { printf("parameter invalid!\n"); return 0; }
	uint32_t addr;
	sscanf(args, "%x", &addr);
	hwaddr_t ans = cmd_page(addr);
	if (ans) printf("Addr is 0x%08x\n", ans);
	return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table[] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{"si","The program executes N instructions step by step and then pauses. When N is not given, the default value is 1.",cmd_si},
	{"info", "info r :Print Register, info w :Print WatchPoint", cmd_info},
	{"x", "Scan memory",cmd_x},
	{"p", "Expression Evaluation", cmd_p},
	{"w", "create WatchPoint", cmd_w},
	{"d", "delete the watchpoint", cmd_d},
	{"bt", "print stack frame", cmd_bt},
	{ "page", "print Physical address of this Linear address", cmd_translate}

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) 
				{ return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
