#include "common.h"
#include <stdlib.h>
#include "burst.h"
#include "memory/tlb.h"
#include "memory/cache.h"
#include "cpu/reg.h"
uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);


/* Memory accessing interfaces */

// hwaddr_read: 从L1缓存中读取数据，如果缓存未命中，则从其他地方读取数据
uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	int32_t set_l1 = (addr >> block_size_bit) & (l1_sets - 1); // 计算L1缓存的set位置
	int32_t i = l1_read(addr);  // 在L1缓存中查找地址
	int32_t imm_l1 = (addr & (block_size - 1)); // 计算块内偏移
	int8_t tmp[block_size * 2]; 
	
	// 如果读取的数据跨越了一个缓存块，处理跨页读取
	if (imm_l1 + len > block_size) {
		// 先读取第一个缓存块的内容
		memcpy(tmp, l1_cache[set_l1][i].block + imm_l1, block_size - imm_l1);
		// 继续读取下一个缓存块
		int32_t i_last = l1_read(addr + block_size - imm_l1);
		int32_t set_last = ((addr + block_size - imm_l1) >> block_size_bit) & (l1_sets - 1);
		memcpy(tmp + block_size - imm_l1, l1_cache[set_last][i_last].block, len - (block_size - imm_l1));
	}
	else
		// 否则直接从当前缓存块读取数据
		memcpy(tmp, l1_cache[set_l1][i].block + imm_l1, len);

	// 将读取的数据转换为uint32_t格式，并返回
	int qwq = 0; 
	uint32_t ret = unalign_rw(tmp + qwq, 4) & (~0u >> ((4 - len) << 3));
	return ret;
}

// hwaddr_write: 向L1缓存中写数据，并更新相关缓存，处理写通过（write-through）策略
void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	l1_write(addr, len, data);  // 写入L1缓存
	// dram_write(addr, len, data); // 这里可以选择写入DRAM (目前注释掉了)
}

// 读取TLB (Translation Lookaside Buffer) 中的条目
int read_tlb(lnaddr_t addr);

// 向TLB中写入新的映射
void write_tlb(lnaddr_t addr, hwaddr_t haaddr);

// cmd_page: 用于分页地址转换，如果启用了保护模式并且分页功能处于开启状态，则会进行分页
hwaddr_t cmd_page(lnaddr_t addr) {	
	if (!cpu.cr0.protect_enable || !cpu.cr0.paging) return addr; // 如果保护模式或分页未开启，直接返回地址
	/* addr = 10 dictionary + 10 page + 12 offset */
	uint32_t dictionary = addr >> 22, page = (addr >> 12) & 0x3ff, offset = addr & 0xfff;
	
	// 获取页表信息
	uint32_t tmp = (cpu.cr3.page_directory_base << 12) + dictionary * 4; // 页目录基地址 + 页目录索引 * 4
	PDE dictionary_, page_;
	dictionary_.val = hwaddr_read(tmp, 4);  // 读取页目录项
	tmp = (dictionary_.page_frame << 12) + page * 4; // 页目录项页框基址 + 页表索引 * 4
	page_.val = hwaddr_read(tmp, 4); // 读取页表项
	
	// 检查页表项的有效性
	if (dictionary_.present != 1) {
		printf("dirctionary present != 1\n");
		return 0;  // 如果页目录项无效，返回0
	}
	if (page_.page_frame != 1) {
		printf("second page table present != 1\n");
		return 0;  // 如果页表项无效，返回0
	}
	return (page_.page_frame << 12) + offset; // 返回转换后的物理地址
}

// page_translate: 使用TLB和页表进行地址转换
hwaddr_t page_translate(lnaddr_t addr) {
	if (!cpu.cr0.protect_enable || !cpu.cr0.paging) return addr; // 如果保护模式或分页未开启，直接返回地址
	/* addr = 10 dictionary + 10 page + 12 offset */
	uint32_t dictionary = addr >> 22;
	uint32_t page = (addr >> 12) & 0x3ff;
	uint32_t offset = addr & 0xfff;
	
	// 查找TLB
	int index = read_tlb(addr);
	if (index != -1) 
		// 如果TLB命中，返回映射的物理地址
		return (tlb[index].data << 12) + offset;

	// 获取页表信息
	uint32_t tmp = (cpu.cr3.page_directory_base << 12) + dictionary * 4;
	PDE dictionary_;
	PTE page_;
	dictionary_.val = hwaddr_read(tmp, 4); // 读取页目录项
	tmp = (dictionary_.page_frame << 12) + page * 4; 
	page_.val = hwaddr_read(tmp, 4); // 读取页表项

	// 检查页表项的有效性
	Assert(dictionary_.present == 1, "dirctionary present");
	Assert(page_.present == 1, "second present");

	// 计算物理地址并更新TLB
	hwaddr_t addr_ = (page_.page_frame << 12) + offset;
	write_tlb(addr, addr_);
	return addr_;
}

// lnaddr_read: 从逻辑地址读取数据
uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
	uint32_t offset = addr & 0xfff;
	
	// 如果读取的数据跨越了一个页面，递归读取数据
	if ((int64_t)(offset + len) > 0x1000) {	
		size_t l = 0xfff - offset + 1;		// 计算跨页的第一部分
		uint32_t down_val = lnaddr_read(addr, l); // 读取第一部分
		uint32_t up_val = lnaddr_read(addr + l, len - l); // 读取第二部分
		return (up_val << (l * 8)) | down_val; // 合并数据并返回
	}
	else {
		// 否则直接进行地址转换并读取数据
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len);
	}
}

// lnaddr_write: 向逻辑地址写数据
void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	assert(len == 1 || len == 2 || len == 4);
	uint32_t offset = addr & 0xfff;
	
	// 如果写入的数据跨越了一个页面，递归写入数据
	if ((int64_t)(offset + len) > 0x1000) {	
		size_t l = 0xfff - offset + 1;		// 计算跨页的第一部分
		lnaddr_write(addr, l, data & ((1 << (l * 8)) - 1)); // 写入第一部分
		lnaddr_write(addr + l, len - l, data >> (l * 8)); // 写入第二部分
	}
	else {
		// 否则直接进行地址转换并写入数据
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_write(hwaddr, len, data);
	}
}

/* 处理段地址转换 */
lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg) { 
	if (cpu.cr0.protect_enable == 0) return addr; // 如果保护模式未开启，直接返回地址
	return cpu.sreg[sreg].base + addr; // 根据段基地址转换
}

// swaddr_read: 从段地址读取数据
uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
	assert(len == 1 || len == 2 || len == 4);
	lnaddr_t lnaddr = seg_translate(addr, len, sreg); // 转换为逻辑地址
	return lnaddr_read(lnaddr, len); // 读取逻辑地址数据
}

// swaddr_write: 向段地址写数据
void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
	assert(len == 1 || len == 2 || len == 4);
	lnaddr_t lnaddr = seg_translate(addr, len, sreg); // 转换为逻辑地址
	lnaddr_write(lnaddr, len, data); // 向逻辑地址写数据
}
