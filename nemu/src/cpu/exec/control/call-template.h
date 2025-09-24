#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	cpu.esp -= 4;
	// push next instr address
	swaddr_write(cpu.esp, 4, cpu.eip + 1 + DATA_BYTE);
	if(op_src->type == OP_TYPE_IMM)
		cpu.eip += op_src->val;
	else
		cpu.eip = op_src->val - op_src->size -1;	
	print_asm_template1();
}

make_instr_helper(si)
make_instr_helper(rm)

#include "cpu/exec/template-end.h" 
