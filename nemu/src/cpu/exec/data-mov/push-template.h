#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
	// must be 4 
	cpu.esp -= 4; 
	swaddr_write(cpu.esp, 4, op_src->val);
	print_asm_template1();	
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
make_instr_helper(rm)
#endif
make_instr_helper(i)


#include "cpu/exec/template-end.h"
