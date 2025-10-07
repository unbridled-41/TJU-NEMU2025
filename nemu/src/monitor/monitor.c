/*
 * NEMU监控器实现文件
 * 负责NEMU模拟器的初始化、重启和用户交互管理
 */

#include "nemu.h"

#define ENTRY_START 0x100000  // 程序入口点地址

/* 外部变量声明 */
extern uint8_t entry[];     // 入口代码
extern uint32_t entry_len;   // 入口代码长度
extern char *exec_file;     // 可执行文件名

/* 函数声明 */
void load_elf_tables(int, char *[]);  // 加载ELF表
void init_regex();                     // 初始化正则表达式
void init_wp_pool();                   // 初始化观察点池
void init_ddr3();                      // 初始化DDR3内存
void init_L1();                        // 初始化L1缓存
void init_L2();                        // 初始化L2缓存

FILE *log_fp = NULL;  // 日志文件指针

/* 初始化日志文件 */
static void init_log() {
	log_fp = fopen("log.txt", "w");
	Assert(log_fp, "Can not open 'log.txt'");
}

/* 显示欢迎信息 */
static void welcome() {
	printf("Welcome to NEMU!\nThe executable is %s.\nFor help, type \"help\"\n",
		exec_file);
}

/*
 * 初始化NEMU监控器
 * 参数: argc - 命令行参数数量
 *       argv - 命令行参数数组
 */
void init_monitor(int argc, char *argv[]) {
	/* Perform some global initialization */

	/* Open the log file. */
	init_log();

	/* Load the string table and symbol table from the ELF file for future use. */
	load_elf_tables(argc, argv);

	/* Compile the regular expressions. */
	init_regex();

	/* Initialize the watchpoint pool. */
	init_wp_pool();

	/* Display welcome message. */
	welcome();
}

#ifdef USE_RAMDISK
/* 初始化内存磁盘 */
static void init_ramdisk() {
	int ret;
	const int ramdisk_max_size = 0xa0000;
	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	Assert(file_size < ramdisk_max_size, "file size(%zd) too large", file_size);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(0), file_size, 1, fp);
	assert(ret == 1);
	fclose(fp);
}
#endif

/* 加载入口代码到内存 */
static void load_entry() {
	int ret;
	FILE *fp = fopen("entry", "rb");
	Assert(fp, "Can not open 'entry'");

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);

	fseek(fp, 0, SEEK_SET);
	ret = fread(hwa_to_va(ENTRY_START), file_size, 1, fp);
	assert(ret == 1);
	fclose(fp);
}

/* 重启虚拟计算机系统 */
void restart() {
	/* Perform some initialization to restart a program */
#ifdef USE_RAMDISK
	/* Read the file with name `argv[1]' into ramdisk. */
	init_ramdisk();
#endif

	/* Read the entry code into memory. */
	load_entry();

	/* Set the initial instruction pointer. */
	cpu.eip = ENTRY_START;
	cpu.esp = (1 << 24);
	/* Set the initial eflags register. */
	cpu.eflags.val = 0x2;

	/* Initialize DRAM. */
	init_ddr3();
	
	/* Initialize cache. */
	init_L1();
	init_L2();


}
