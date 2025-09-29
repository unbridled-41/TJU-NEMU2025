#include <stdlib.h>
#include "common.h"
#include "burst.h"
#include "misc.h"
extern void ddr3_read(hwaddr_t, uint8_t *);
extern void ddr3_write(hwaddr_t, uint8_t *, uint8_t *);
typedef struct{
	uint8_t valid;
	uint8_t dirty;
	uint32_t tag;
	uint8_t *buf;
} cache_line;

void Ln_read(hwaddr_t addr, uint8_t *buf){
	int i;
	for(i = 0;i < 8; i++){
		ddr3_read(addr + 8 * i, buf + 8 * i);
	}
}
void Ln_write(hwaddr_t addr, uint8_t *buf){
	uint8_t mask[BURST_LEN] = {1,1,1,1,1,1,1,1};
	int i;
	for(i = 0;i < 8; i++){
		ddr3_write(addr + 8 * i, buf + 8 * i, mask);
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

