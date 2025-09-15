#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

/* block */
/* Cache block size parameters */
#define block_size_bit 6        // 地址的位移量，表示64字节块大小
#define block_size 64           // 单个缓存块的大小（字节）

/* Block结构体：每个缓存块包含标记（tag），数据块（block），有效位（valid）和脏位（dirty） */
typedef struct {
    uint32_t tag;                  // 缓存块的标记部分（19位）
    uint8_t block[block_size];     // 缓存数据块（64字节）
    bool valid, dirty;             // 有效位（valid）和脏位（dirty）
} block;

/* L1 Cache Configuration */
/* L1缓存：采用写透策略，且不进行写分配（write-through, no-write-allocate） */
#define l1_ways 8                   // 每个set包含的块数（8路）
#define l1_sets_bit 7               // 地址中的set部分的位数
#define l1_sets (1024/8)            // L1缓存中set的总数（1024字节/每set 8块）
block l1_cache[l1_sets][l1_ways];  // L1缓存数组，每个set有8块
uint64_t l1_t;                     // L1缓存的访问时间计数器
#define l1_tag_bit 19               // L1缓存标记部分的位数（19位）


/* L2 Cache Configuration */
/* L2缓存：采用写回策略，且进行写分配（write-back, write-allocate） */
#define l2_ways 16                  // 每个set包含的块数（16路）
#define l2_sets_bit 12              // 地址中的set部分的位数
#define l2_sets (64*1024/16)        // L2缓存中set的总数（64KB / 每set 16块）
block l2_cache[l2_sets][l2_ways];  // L2缓存数组，每个set有16块
uint64_t l2_t;                     // L2缓存的访问时间计数器
#define l2_tag_bit 14               // L2缓存标记部分的位数（14位）

/* 函数声明 */

/* 初始化L1和L2缓存 */
void init_cache();

/* 计算缓存的统计信息（访问时间等） */
void cache_t();

/* 读取L1缓存的数据 */
int32_t l1_read(hwaddr_t addr);

/* 读取L2缓存的数据 */
int32_t l2_read(hwaddr_t addr);

/* 向L1缓存写数据 */
void l1_write(hwaddr_t addr, size_t len, uint32_t data);

/* 向L2缓存写数据 */
void l2_write(hwaddr_t addr, size_t len, uint32_t data);

#endif
