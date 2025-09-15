#include "memory/tlb.h"  // 引入TLB头文件
#include "burst.h"       // 引入其他所需的头文件
#include <time.h>        // 引入时间函数库
#include <stdlib.h>      // 引入标准库，包含随机数生成函数

// 初始化TLB
void init_tlb() {
    int i;
    for (i = 0; i < TLB_SIZE; i++) {
        tlb[i].valid = 0;  // 将所有TLB条目的有效位设为0
    }
    srand(clock());  // 使用当前时间作为随机数种子
}

// 从TLB读取条目，根据地址返回条目的索引，如果没有找到返回-1
int read_tlb(lnaddr_t addr) {
    int tag = addr >> 12;  // 计算标签，取高20位
    int i;
    for (i = 0; i < TLB_SIZE; ++i) {
        // 检查TLB条目是否有效且标签匹配
        if (tlb[i].tag == tag && tlb[i].valid) return i; // 找到匹配的条目
    }
    return -1; // 如果TLB中没有找到匹配的条目，返回-1
}

// 将地址写入TLB
void write_tlb(lnaddr_t addr, hwaddr_t addr_) {
    int tag = addr >> 12;  // 计算标签，取高20位
    addr_ >>= 12;          // 将物理地址右移12位，获取页框号
    int i;
    for (i = 0; i < TLB_SIZE; i++) {
        // 找到一个无效的条目
        if (!tlb[i].valid) {
            tlb[i].tag = tag;      // 设置标签
            tlb[i].data = addr_;   // 设置数据
            tlb[i].valid = 1;      // 标记为有效
            return;
        }
    }
    // 如果TLB已满，随机选择一个条目进行替换
    i = rand() % TLB_SIZE;      // 随机生成一个索引
    tlb[i].tag = tag;           // 替换条目的标签
    tlb[i].data = addr_;        // 替换条目的数据
    tlb[i].valid = 1;           // 标记为有效
}
