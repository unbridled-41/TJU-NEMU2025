#include "cpu/exec/template-start.h"

#if DATA_BYTE == 1

#define ZF (cpu.eflags.ZF)
#define SF (cpu.eflags.SF)
#define CF (cpu.eflags.CF)
#define OF (cpu.eflags.OF)
#define PF (cpu.eflags.PF)
#endif

#define make_setcc_helper(cond, name) \
	static void concat4(do_set, name, _, SUFFIX)(){ \
	bool value = (cond) ; \
	OPERAND_W(op_src, value); \
	print_asm_template1(); \
} \
make_helper(concat4(set, name, _, SUFFIX)){ \
	return idex(eip, concat(decode_rm_, SUFFIX), concat4(do_set, name, _, SUFFIX)); \
} 

make_setcc_helper(!CF&!ZF,a)
make_setcc_helper(!CF,ae)
make_setcc_helper(CF,b)
make_setcc_helper(CF|ZF,be)
make_setcc_helper(ZF,e)
make_setcc_helper(!ZF&(SF==OF),g)
make_setcc_helper(ZF|(SF==OF),ge)
make_setcc_helper(SF!=OF,l)
make_setcc_helper(ZF|(SF!=OF),le)
make_setcc_helper(!ZF,ne)
make_setcc_helper(!OF,no)	
make_setcc_helper(!PF,np)
make_setcc_helper(!SF,ns)
make_setcc_helper(OF,o)
make_setcc_helper(PF,p)
make_setcc_helper(SF,s)

#if DATA_BYTE == 1

#undef ZF
#undef SF
#undef CF
#undef OF
#undef PF
#endif

#include "cpu/exec/template-end.h" 
