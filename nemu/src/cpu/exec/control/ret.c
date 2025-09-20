#include "cpu/exec/helper.h"

make_helper(ret) {
	//pop caller next instr address
	swaddr_t addr = swaddr_read(cpu.esp, 4);
	cpu.eip = addr;
	cpu.esp += 4;
	print_asm("ret");
	return 0;
}