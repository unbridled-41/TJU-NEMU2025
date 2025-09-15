#ifndef __TLB_H__
#define __TLB_H__

#include "common.h"  // 包含通用的定义和类型

#define TLB_SIZE 64  // 定义TLB的大小

// TLB（翻译后备缓冲区）条目的结构体定义
typedef struct {
    bool valid;         // 有效位，表示该条目是否有效
    uint32_t tag;      // 标签，用于标识页
    uint32_t data;     // 数据，存储对应物理页的地址
} TLB;

// 定义一个TLB数组，大小为TLB_SIZE
TLB tlb[TLB_SIZE];

// 函数声明：初始化TLB
void init_tlb();

// 函数声明：根据线性地址读取TLB
int read_tlb(lnaddr_t addr);

// 函数声明：根据线性地址写入TLB
void write_tlb(lnaddr_t addr, hwaddr_t haaddr);

#endif // __TLB_H__
