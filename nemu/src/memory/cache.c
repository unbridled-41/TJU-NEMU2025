#include <stdlib.h>
#include "common.h"
#include "burst.h"
#include "misc.h"

typedef struct{
	uint8_t valid;
	uint8_t dirty;
	uint32_t tag;
	uint8_t *buf;
} cache_line;

void miss_read(hwaddr_t addr, uint8_t *buf){
}
void miss_write(hwaddr_t addr, uint8_t *buf){
}

#define s 7
#define b 6
#define E 8
#define use_dirty 0
#define cache L1
#include "cache-template.h"
#undef s
#undef b
#undef E
#undef use_dirty
#undef cache

#define s 12
#define b 6
#define E 16
#define use_dirty 1
#define cache L2
#include "cache-template.h"
#undef s
#undef b
#undef E
#undef use_dirty
#undef cache