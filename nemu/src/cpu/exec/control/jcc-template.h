#include "cpu/exec/template-start.h"

#define cond e

static void concat4(do_j, cond, _, SUFFIX)(){
	if(!(cpu.eflags.ZF == 1))return ;
	cpu.eip += op_src->val;
	print_asm("j" str(cond) " %x", cpu.eip + 1 + DATA_BYTE);}


make_helper(concat4(j, cond, _, SUFFIX)){
	return idex(eip, concat(decode_si_, SUFFIX), concat4(do_j, cond, _, SUFFIX));
}


#include "cpu/exec/template-end.h"