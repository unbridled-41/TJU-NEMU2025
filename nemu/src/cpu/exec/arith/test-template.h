#include "cpu/exec/template-start.h"
#include "cpu/eflags.h"
#define instr test

static void do_execute() {
    DATA_TYPE_S result = op_src->val & op_dest->val;      
    cpu.eflags.CF = 0;                                        
    cpu.eflags.OF = 0;                                        
    /*cpu.eflags.SF = result >> (DATA_BYTE * 8 - 1);
    cpu.eflags.ZF = !result;
    int temp_ = 0;
    int i;
    for (i = 0; i < 8; i++) temp_ += (result >> i) & 1;
    if (temp_ % 2) cpu.eflags.PF = 0;
    else cpu.eflags.PF = 1;*/
    update_eflags_pf_zf_sf( (uint32_t)result );


    print_asm_template2();
}

make_instr_helper(r2rm)
make_instr_helper(i2rm)
#include "cpu/exec/template-end.h"