#include "cpu/exec/helper.h"
 
make_helper(ret) {
	//pop caller next instr address
	swaddr_t addr = swaddr_read(cpu.esp, 4);
	cpu.eip = addr - 1;
	cpu.esp += 4;
	print_asm("ret");
	return 1;
}
#define instr ret
#define SUFFIX w
make_helper(ret_i_w) {
	int len = decode_i_w(eip + 1);
	//pop caller next instr address
	swaddr_t addr = swaddr_read(cpu.esp, 4);
	cpu.eip = addr - 1 - len;
	cpu.esp += 4;
	// pop imm16 bytes
	cpu.esp += (op_src->val);
	print_asm_template1();
	return len + 1;
}
