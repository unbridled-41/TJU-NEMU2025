#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	cpu.esp -= 4;
	// push next instr address
	swaddr_write(cpu.esp, 4, cpu.eip + 1 + DATA_BYTE);	
	cpu.eip += op_src->val;
	print_asm_template1();
}

make_instr_helper(si)
make_helper(concat(call_rm_, SUFFIX)) {
	cpu.esp -= 4;
	int len = concat(decode_rm_, SUFFIX)(eip + 1);
	// push next instr address
	swaddr_write(cpu.esp, 4, cpu.eip + 1 + len);
	cpu.eip = op_src->val - (len + 1);
	print_asm(str(instr) " *%s", op_src->str);
	return len + 1;
}
#include "cpu/exec/template-end.h" 
