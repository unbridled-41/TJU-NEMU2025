#include "cpu/exec/template-start.h"

#define instr lgdt

static void do_execute() {
	/* Ŀ���ַ���� 6Bytes �����ݣ����limit��base */
	// 16λ��������16bits limit + 24bits base | 2+3 Bytes
	// 32λ��������16bits limit + 32bits base | 2+4 Bytes(����ֽڴ�� ��λ��ַλ)
	cpu.GDTR.limit = swaddr_read(op_src->addr, 2, R_DS);
	if (op_src->size == 2)
		cpu.GDTR.base = swaddr_read(op_src->addr + 2, 3, R_DS);
	else {
		cpu.GDTR.base = swaddr_read(op_src->addr + 2, 4, R_DS);
	}
	print_asm_template1();
}

make_instr_helper(rm);

#include "cpu/exec/template-end.h"