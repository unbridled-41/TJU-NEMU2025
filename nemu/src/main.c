#include "nemu.h"

/* 函数声明 */
void init_monitor(int, char *[]);  // 初始化NEMU监控器
void reg_test();                   // 测试CPU寄存器实现
void restart();                    // 重启虚拟计算机系统
void ui_mainloop();                // 用户交互主循环

int main(int argc, char *argv[]) {

	/* Initialize the monitor. */
	init_monitor(argc, argv);

	/* Test the implementation of the `CPU_state' structure. */
	reg_test();

	/* Initialize the virtual computer system. */
	restart();

	/* Receive commands from user. */
	ui_mainloop();

	return 0;
}