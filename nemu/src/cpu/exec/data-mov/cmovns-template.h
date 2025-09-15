#include "cpu/exec/template-start.h"

#define instr cmovns

static void do_execute() {
	if (cpu.eflags.SF == 0) {
		OPERAND_W(op_dest, op_src->val);
	}
	print_asm_template2();
}

make_instr_helper(r2rm)
make_instr_helper(rm2r)
#undef instr

#define instr cmovs

static void do_execute() {
	if (cpu.eflags.SF == 1) {
		OPERAND_W(op_dest, op_src->val);
	}
	print_asm_template2();
}

make_instr_helper(r2rm)
make_instr_helper(rm2r)
#undef instr
#include "cpu/exec/template-end.h"