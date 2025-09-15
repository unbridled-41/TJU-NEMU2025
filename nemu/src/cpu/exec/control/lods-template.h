#include "cpu/exec/template-start.h"

#define instr lods
make_helper(concat(lods_p_, SUFFIX)) {
	REG(0) = MEM_R(reg_l(6), R_DS);
	if (cpu.eflags.DF == 0) 
		reg_l(6) += DATA_BYTE;
	else 
		reg_l(6) -= DATA_BYTE;
	print_asm("lods");
	return 1;
}



#include "cpu/exec/template-end.h"
