#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	cpu.esp -= 4;
	// push next instr address
	swaddr_write(cpu.esp, 4, cpu.eip + 1 + DATA_BYTE);
	cpu.eip += op_src->simm;
	print_asm_template1();
}

make_instr_helper(si)

#include "cpu/exec/template-end.h" 