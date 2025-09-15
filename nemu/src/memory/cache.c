#include "memory/cache.h"
#include "burst.h"
#include <stdlib.h>
#include <time.h>
#include <cpu/reg.h>

void ddr3_read_2_cache(hwaddr_t addr, void* data);
void ddr3_write_2_cache(hwaddr_t addr, void* data, uint8_t* mask);

void init_cache() {
	// 初始化 L1 和 L2 缓存，将所有缓存块的有效位设置为 false，表示缓存为空。
	l1_t = 0;	
	int i, j;	
	srand(clock());
	for (i = 0; i < l1_sets; i++) {
		for (j = 0; j < l1_ways; j++) {
			l1_cache[i][j].valid = false;  // 设置 L1 缓存无效
		}
	}
	for (i = 0; i < l2_sets; i++) {
		for (j = 0; j < l2_ways; j++) {
			l2_cache[i][j].valid = false;  // 设置 L2 缓存无效
			l2_cache[i][j].dirty = false;  // 设置 L2 缓存为干净
		}
	}
}

int32_t l1_read(hwaddr_t addr) {
	// 从 L1 缓存读取数据。如果命中则返回缓存索引，如果未命中则从 L2 读取
	int32_t tag_l1 = (addr >> (l1_sets_bit + block_size_bit));
	int32_t set_l1 = (addr >> block_size_bit) & (l1_sets - 1);
	int i;
	for (i = 0; i < l1_ways; i++) {	// 遍历每个缓存块
		if (!l1_cache[set_l1][i].valid) 
			continue;
		if (l1_cache[set_l1][i].tag == tag_l1) {
			l1_t += 2; 
			return i;  // 命中，返回缓存索引
		}
	}
	// 如果未命中，从 L2 缓存读取数据
	for (i = 0; i < l1_ways; i++) {
		if (!l1_cache[set_l1][i].valid) 
			break;
	}
	if (i == l1_ways) 
		i = rand() % l1_ways;  // 替换策略
	l1_cache[set_l1][i].valid = true; 
	l1_cache[set_l1][i].tag = tag_l1;
	int set_l2 = (addr >> block_size_bit) & (l2_sets - 1); 
	int ii = l2_read(addr);  // 从 L2 读取数据
	memcpy(l1_cache[set_l1][i].block, l2_cache[set_l2][ii].block, block_size);
	l1_t += 200; 
	return i;
}

int32_t l2_read(hwaddr_t addr) {
	// 从 L2 缓存读取数据。如果命中则返回缓存索引，如果未命中从主存读取
	int32_t tag_l2 = (addr >> (l2_sets_bit + block_size_bit));
	int32_t set_l2 = (addr >> block_size_bit) & (l2_sets - 1);
	int i = 0;
	for (i = 0; i < l2_ways; i++) {	// 遍历每个缓存块
		if (!l2_cache[set_l2][i].valid)
			continue;
		if (l2_cache[set_l2][i].tag == tag_l2) {
			l2_t += 2; 
			return i;  // 命中，返回缓存索引
		}
	}
	// 如果未命中，从主存读取数据
	for (i = 0; i < l2_ways; i++) {
		if (!l2_cache[set_l2][i].valid)
			break;
	}
	if (i == l2_ways)
		i = rand() % l2_ways;  // 替换策略
	if (l2_cache[set_l2][i].valid && l2_cache[set_l2][i].dirty) {
		// 如果缓存块是脏的，需要写回到主存
		uint8_t tmp[BURST_LEN * 2];
		memset(tmp, 1, sizeof(tmp));
		int j;
		uint32_t addr_pre = ((l2_cache[set_l2][i].tag << (l2_sets_bit + block_size_bit)) | (set_l2 << block_size_bit));
		for (j = 0; j < block_size / BURST_LEN; j++) {
			ddr3_write_2_cache(addr_pre + BURST_LEN * j, l2_cache[set_l2][i].block + BURST_LEN * j, tmp);
		}
	}
	l2_cache[set_l2][i].valid = true;
	l2_cache[set_l2][i].tag = tag_l2;
	l1_t += 200;
	int j;
	for (j = 0; j < block_size / BURST_LEN; j++) {
		ddr3_read_2_cache(((addr >> block_size_bit) << block_size_bit) + BURST_LEN * j, l2_cache[set_l2][i].block + BURST_LEN * j);
	}
	return i;
}

void l1_write(hwaddr_t addr, size_t len, uint32_t data) {
	// 在 L1 缓存中写数据，使用写透策略，不进行写分配
	int32_t tag_l1 = (addr >> (l1_sets_bit + block_size_bit));
	int32_t set_l1 = (addr >> block_size_bit) & (l1_sets - 1);
	int32_t imm_l1 = (addr & (block_size - 1));
	bool hit = false; 
	int i;
	for (i = 0; i < l1_ways; i++) {	
		if (!l1_cache[set_l1][i].valid) continue;
		if (l1_cache[set_l1][i].tag == tag_l1) {
			hit = true; 
			break;
		}
	}
	if (hit) {  // 如果命中
		if (imm_l1 + len <= block_size) {
			memcpy(l1_cache[set_l1][i].block + imm_l1, &data, len);  // 写入 L1
		}
		else {	
			memcpy(l1_cache[set_l1][i].block + imm_l1, &data, block_size - imm_l1);  // 写入部分块
			l1_write(addr + block_size - imm_l1, len - (block_size - imm_l1), data >> 8 * (block_size - imm_l1));  // 继续写入剩余数据
		}
		l1_t += 2;
	}
	else 
		l1_t += 200;  // 未命中
	l2_write(addr, len, data);  // 写入 L2
}

void l2_write(hwaddr_t addr, size_t len, uint32_t data) {
	// 在 L2 缓存中写数据，使用写回策略，进行写分配
	int32_t tag_l2 = (addr >> (l2_sets_bit + block_size_bit));
	int32_t set_l2 = (addr >> block_size_bit) & (l2_sets - 1);
	int32_t imm_l2 = (addr & (block_size - 1));
	bool hit = false; 
	int i;
	for (i = 0; i < l2_ways; i++) {	
		if (!l2_cache[set_l2][i].valid) continue;
		if (l2_cache[set_l2][i].tag == tag_l2) {
			hit = true; 
			break;
		}
	}
	if (hit) {  // 如果命中
		l2_t += 2; 
		l2_cache[set_l2][i].dirty = true;
		if (imm_l2 + len <= block_size) {
			memcpy(l2_cache[set_l2][i].block + imm_l2, &data, len);  // 写入 L2
		}
		else {	
			memcpy(l2_cache[set_l2][i].block + imm_l2, &data, block_size - imm_l2);  // 写入部分块
			l2_write(addr + block_size - imm_l2, len - (block_size - imm_l2), data >> 8 * (block_size - imm_l2));  // 继续写入剩余数据
		}
	}
	else {  // 如果未命中
		i = l2_read(addr);  // 从 L2 读取数据
		l2_write(addr, len, data);  // 写入 L2
	}
}
