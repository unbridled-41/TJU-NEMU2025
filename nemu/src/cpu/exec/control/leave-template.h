#include "cpu/exec/template-start.h"

#define instr leave

//make_helper(concat(leave_n_, SUFFIX)) {
//	REG(4) = REG(5);
//	POP(REG(5));
//
//	print_asm_template1();
//	return 1;
//	
//}
static void do_execute() {
	swaddr_t i;
	for (i = REG(R_ESP); i < REG(R_EBP); i += DATA_BYTE)
		MEM_W(i, 0, 2);
	REG(R_ESP) = REG(R_EBP);
	REG(R_EBP) = MEM_R(REG(R_ESP), R_SS);
	REG(R_ESP) += DATA_BYTE;
	print_asm("leave");
}
make_instr_helper(r)



#include "cpu/exec/template-end.h"
