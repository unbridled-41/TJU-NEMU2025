#include "nemu.h"
#include <stdlib.h>
#include <time.h>

CPU_state cpu;

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
	srand(time(0));
	uint32_t sample[8];
	uint32_t eip_sample = rand();
	cpu.eip = eip_sample;

	int i;
	for(i = R_EAX; i <= R_EDI; i ++) {
		sample[i] = rand();
		reg_l(i) = sample[i];
		assert(reg_w(i) == (sample[i] & 0xffff));
	}

	assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
	assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
	assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
	assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
	assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
	assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
	assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
	assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

	assert(sample[R_EAX] == cpu.eax);
	assert(sample[R_ECX] == cpu.ecx);
	assert(sample[R_EDX] == cpu.edx);
	assert(sample[R_EBX] == cpu.ebx);
	assert(sample[R_ESP] == cpu.esp);
	assert(sample[R_EBP] == cpu.ebp);
	assert(sample[R_ESI] == cpu.esi);
	assert(sample[R_EDI] == cpu.edi);

	assert(eip_sample == cpu.eip);
}

void sreg_set(uint8_t id) {	
    // 根据给定的段寄存器 ID 设置段寄存器的信息

    // 计算描述符表中对应段选择子的地址
    lnaddr_t chart_addr = cpu.GDTR.base + ((cpu.sreg[id].selector >> 3) << 3);
    
    // 从内存中读取段描述符的前两个字段（基址和限制）
    sreg_info.p1 = lnaddr_read(chart_addr, 4);      // 读取段描述符的低32位（基址的低部分和一些属性）
    sreg_info.p2 = lnaddr_read(chart_addr + 4, 4);  // 读取段描述符的高32位（基址的高部分和限制）

    // 计算段基址，合并段描述符中的基址部分
    cpu.sreg[id].base = sreg_info.b1 + (sreg_info.b2 << 16) + (sreg_info.b3 << 24);
    
    // 计算段限制，合并段描述符中的限制部分
    cpu.sreg[id].limit = sreg_info.lim1 + (sreg_info.lim2 << 16) + (0xfff << 24);
    
    // 检查 G 位（粒度），如果为 1，则将限制乘以 4096（即左移 12 位）
    if (sreg_info.g == 1) {  // G = 0 表示限制以字节为单位，G = 1 表示以 4KB 页为单位
        cpu.sreg[id].limit <<= 12; // 将限制转换为以 4KB 为单位
    }
}


