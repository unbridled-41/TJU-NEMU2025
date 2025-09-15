#include "cpu/exec/template-start.h"
#include "cpu/eflags.h"
#include "cpu/decode/decode.h"
#define instr cmp

static void do_execute() {
    //DATA_TYPE result = op_dest->val - op_src->val;
    DATA_TYPE_S result1 = op_dest->val - op_src->val;

    if (op_dest->val < op_src->val) {
        cpu.eflags.CF = 1;
    }
    else {
        cpu.eflags.CF = 0;
    }
    update_eflags_pf_zf_sf( (uint32_t)result1);
    int a = op_dest->val >> (DATA_BYTE * 8 - 1);
    int b = op_src->val >> (DATA_BYTE * 8 - 1);
    cpu.eflags.OF = (a != b) && (b == cpu.eflags.SF);
    //int temp_ = 0;
    //int i;
    if (((op_dest->val & 0xf) - (op_src->val & 0xf)) >> 4) {
        cpu.eflags.AF = 1;
    }
    else {
        cpu.eflags.AF = 0;
    }
    print_asm_template2();
}
#if DATA_BYTE != 1
make_instr_helper(si2rm)
#endif
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)
make_instr_helper(i2a)


#include "cpu/exec/template-end.h"