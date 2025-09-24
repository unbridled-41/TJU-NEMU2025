#include "cpu/exec/helper.h"
 
make_helper(ret) {
	//pop caller next instr address
	swaddr_t addr = swaddr_read(cpu.esp, 4);
	cpu.eip = addr - 1;
	cpu.esp += 4;
	print_asm("ret");
	return 1;
}

make_helper(ret_i_w) {
	//pop caller next instr address
	swaddr_t addr = swaddr_read(cpu.esp, 4);
	cpu.eip = addr - 1;
	cpu.esp += 4;
	// pop imm16 bytes
	int len = decode_i_w(eip + 1);
	cpu.esp += 4*(op_src->val);
	print_asm_template1();
	return len + 1;
}
