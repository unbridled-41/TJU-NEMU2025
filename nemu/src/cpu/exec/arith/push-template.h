#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {//ջ��
	op_src->val = op_src->val;
	reg_l(R_SP) -= 4;                       //Ϊ��pop���㣬ͳһ-4
	swaddr_write(reg_l(R_SP), 4, (DATA_TYPE_S)op_src->val, R_SS);
	print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)
make_instr_helper(i)
#include "cpu/exec/template-end.h"
