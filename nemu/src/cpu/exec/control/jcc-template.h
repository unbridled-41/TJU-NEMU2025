#include "cpu/exec/template-start.h"

#if DATA_BYTE == 1
static void do_je(){
	if(!(cpu.eflags.ZF == 1))return ;
	cpu.eip += op_src->val;
	print_asm(str(instr) " %x", cpu.eip + 1 + DATA_BYTE);
}
#endif

make_helper(concat(je_,SUFFIX)){
	return idex(eip, concat(decode_si_, SUFFIX), concat(do_, je));
}


#include "cpu/exec/template-end.h"