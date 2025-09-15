#include "cpu/exec/template-start.h"
#include "cpu/eflags.h"
#define instr add
static void do_execute() {
	DATA_TYPE_S result = op_dest->val + op_src->val;
	OPERAND_W(op_dest, result);
	
	cpu.eflags.CF = (result < op_dest->val);
	
	update_eflags_pf_zf_sf((uint32_t)result);
	
	int a = op_dest->val >> (DATA_BYTE * 8 - 1);
	int b = op_src->val >> (DATA_BYTE * 8 - 1);
	
	cpu.eflags.OF = (a == b) && (b != cpu.eflags.SF);
	
	print_asm_template2();
}

#if DATA_BYTE != 1
make_instr_helper(si2rm)
#endif
make_instr_helper(rm2r)
make_instr_helper(r2rm)
make_instr_helper(i2rm)
#undef instr
#define instr addEAX

static void do_execute() {
	DATA_TYPE_S result = REG(0) + op_src->val;
	REG(0) = result;

	cpu.eflags.CF = (result < op_dest->val);

	update_eflags_pf_zf_sf((uint32_t)result);

	int a = op_dest->val >> (DATA_BYTE * 8 - 1);
	int b = op_src->val >> (DATA_BYTE * 8 - 1);

	cpu.eflags.OF = (a == b) && (b != cpu.eflags.SF);

	print_asm_template2();
}


make_instr_helper(i)
#undef instr
#include "cpu/exec/template-end.h"
