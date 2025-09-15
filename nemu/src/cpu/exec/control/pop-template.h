#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {//Õ»¶Î
   
    OPERAND_W(op_src, MEM_R(REG(R_ESP), R_SS));
    MEM_W(REG(R_ESP), 0, 2);
    REG(R_ESP) += 4;
    print_asm_template1();
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"
