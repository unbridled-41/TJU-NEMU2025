#include "cpu/exec/template-start.h"

#if DATA_BYTE == 1

#define ZF (cpu.eflags.ZF)
#define SF (cpu.eflags.SF)
#define CF (cpu.eflags.CF)
#define OF (cpu.eflags.OF)
#define PF (cpu.eflags.PF)
#endif

#define make_jcc_helper(cond, name) \
	static void concat4(do_j, name, _, SUFFIX)(){ \
	if(!(cond))return ; \
	cpu.eip += op_src->simm; \
	print_asm("j" str(name) " %x", cpu.eip + 1 + DATA_BYTE); \
} \
make_helper(concat4(j, name, _, SUFFIX)){ \
	return idex(eip, concat(decode_si_, SUFFIX), concat4(do_j, name, _, SUFFIX)); \
} 

make_jcc_helper(!CF&!ZF,a)
make_jcc_helper(!CF,ae)
make_jcc_helper(CF,b)
make_jcc_helper(CF|ZF,be)
make_jcc_helper(!cpu.ecx,ecxz)
make_jcc_helper(ZF,e)
make_jcc_helper(!ZF&(SF==OF),g)
make_jcc_helper(ZF|(SF==OF),ge)
make_jcc_helper(SF!=OF,l)
make_jcc_helper(ZF|(SF!=OF),le)
make_jcc_helper(!ZF,ne)
make_jcc_helper(!OF,no)	
make_jcc_helper(!PF,np)
make_jcc_helper(!SF,ns)
make_jcc_helper(OF,o)
make_jcc_helper(PF,p)
make_jcc_helper(SF,s)

#if DATA_BYTE == 4

#undef ZF
#undef SF
#undef CF
#undef OF
#undef PF
#endif

#include "cpu/exec/template-end.h" 
