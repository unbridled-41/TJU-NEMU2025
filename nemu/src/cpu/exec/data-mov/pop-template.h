#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	uint32_t val = swaddr_read(cpu.esp, DATA_BYTE);
	if(op_src->type == OP_TYPE_REG)
		REG(op_src->reg) = val;
	else if(op_src->type == OP_TYPE_MEM)
	      swaddr_write(op_src->addr,DATA_BYTE, val);
	else 
	      assert(0);
    OPERAND_W(op_src, MEM_R(cpu.esp));
	cpu.esp += DATA_BYTE;
	print_asm_template1();
}

make_instr_helper(r);

#include "cpu/exec/template-end.h"