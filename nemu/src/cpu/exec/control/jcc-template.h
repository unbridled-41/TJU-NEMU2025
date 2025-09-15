#include "cpu/exec/template-start.h"

#define instr jbe
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if (cpu.eflags.ZF == 1 || cpu.eflags.CF == 1) { 
        cpu.eip += (int32_t)imm; 
    }
    print_asm("jbe %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
#undef instr

#define instr jnz
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if (cpu.eflags.ZF == 0 ) {
        cpu.eip += (int32_t)imm;
    }
    print_asm("jnz %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
#undef instr

#define instr jle
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if ((cpu.eflags.ZF == 1) || (cpu.eflags.SF != cpu.eflags.OF)) {
        cpu.eip += (int32_t)imm;
    }
    print_asm("jle %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
#undef instr

#define instr jg
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if ((cpu.eflags.ZF == 0) && (cpu.eflags.SF == cpu.eflags.OF)) {
        cpu.eip += (int32_t)imm;
    }
    print_asm("jg %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
make_instr_helper(i)
#undef instr

#define instr jl
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if ( cpu.eflags.SF != cpu.eflags.OF) {
        cpu.eip += (int32_t)imm;
    }
    print_asm("jl %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
#undef instr

#define instr jge
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if (cpu.eflags.SF == cpu.eflags.OF ) {
        cpu.eip += (int32_t)imm;
    }
    print_asm("jge %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
#undef instr

#define instr ja
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if (cpu.eflags.CF == 0 && cpu.eflags.ZF == 0) {
        cpu.eip += (int32_t)imm;
    }
    print_asm("ja %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
#undef instr

#define instr jne
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if (cpu.eflags.ZF == 0 ) {
        cpu.eip += (int32_t)imm;
    }
    print_asm("jne %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
#undef instr

#define instr js
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if (cpu.eflags.SF == 1) {
        cpu.eip += (int32_t)imm;
    }
    print_asm("jne %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
#undef instr

#define instr jns
static void do_execute() {
    DATA_TYPE_S imm = op_src->val;
    if (cpu.eflags.SF == 0) {
        cpu.eip += (int32_t)imm;
    }
    print_asm("jne %x", cpu.eip + DATA_BYTE + 1);
}
make_instr_helper(si)
#undef instr
#include "cpu/exec/template-end.h"