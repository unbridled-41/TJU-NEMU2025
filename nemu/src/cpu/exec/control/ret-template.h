
#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_n_, SUFFIX)) {
	uint32_t temp = swaddr_read(reg_l(4), 4, 2);
	reg_l(4) += 4;
	cpu.eip = temp - 1;
	print_asm(str(instr));
	return 1;
}

static void do_execute() {
	uint32_t temp = swaddr_read(reg_l(4), 4, 2);
	reg_l(4) += 4;
	cpu.eip = temp ;
	DATA_TYPE_S qwq = op_src->val;
	reg_l(4) += qwq;
	print_asm_template1();
}


make_instr_helper(i)
#include "cpu/exec/template-end.h"
