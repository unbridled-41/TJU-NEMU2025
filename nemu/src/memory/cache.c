#include <stdlib.h>
#include "common.h"
#include "burst.h"
#include "misc.h"
extern void ddr3_read(hwaddr_t, uint8_t *);
extern void ddr3_write(hwaddr_t, uint8_t *, uint8_t *);
uint8_t all_mask[2 * 64];
typedef struct{
	uint8_t valid;
	uint8_t dirty;
	uint32_t tag;
	uint8_t *buf;
} cache_line;

void Ln_read(hwaddr_t addr, uint8_t *buf){
	int i;
        addr &= ~63;
	for(i = 0;i < 8; i++){
		ddr3_read(addr + 8 * i, buf + 8 * i);
	}
}

void Ln_write(hwaddr_t addr, uint8_t *buf, uint8_t *mask){
	int i;
        addr &= ~63;
	for(i = 0;i < 8; i++){
		ddr3_write(addr + 8 * i, buf + 8 * i, mask + 8 * i);
	}
}


#define miss_read  Ln_read
#define miss_write  Ln_write
#define s 12
#define b 6
#define E 16
#define use_dirty 1
#define cache L2
#include "cache-template.h"
#undef miss_read
#undef miss_write
#undef s
#undef b
#undef E
#undef use_dirty
#undef cache


#define miss_read  L2_read
#define miss_write  L2_write
#define s 7
#define b 6
#define E 8
#define use_dirty 0 
#define cache L1
#include "cache-template.h"
#undef miss_read
#undef miss_write
#undef s
#undef b
#undef E
#undef use_dirty
#undef cache

uint32_t cache_read(hwaddr_t addr, size_t len) {
	uint32_t offset = addr & 0x3f;
        addr -= offset;
	uint8_t temp[2 * 64];
	
	L1_read(addr, temp);

	if(offset + len > 64) {
		/* data cross the burst boundary */
		L1_read(addr + 64, temp + 64);
	}

	return unalign_rw(temp + offset, 4);
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t offset = addr & 0x3f;
	addr -= offset;
        uint8_t temp[2 * 64];
	uint8_t mask[2 * 64];
	memset(mask, 0, 2 * 64);

	*(uint32_t *)(temp + offset) = data;
	memset(mask + offset, 1, len);

	L1_write(addr, temp, mask);

	if(offset + len > 64) {
		/* data cross the burst boundary */
		L1_write(addr + 64, temp + 64, mask + 64);
	}
}
