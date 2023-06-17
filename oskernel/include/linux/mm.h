/*
 * 内存检测，打印内存信息
 * */

#ifndef OS_MM_H
#define OS_MM_H


#ifndef ZIYA_OSKERNEL_TEACH_MM_H
#define ZIYA_OSKERNEL_TEACH_MM_H

#include "types.h"

#define PAGE_SIZE 4096

/*
 * 地址范围描述符结构
 * */
typedef struct {
    unsigned int  base_addr_low;    //内存基地址的低32位
    unsigned int  base_addr_high;   //内存基地址的高32位
    unsigned int  length_low;       //内存块长度的低32位
    unsigned int  length_high;      //内存块长度的高32位
    unsigned int  type;             //描述内存块的类型
}check_memmory_item_t;

typedef struct {
    unsigned short          times;
    check_memmory_item_t*   data;
}check_memory_info_t;

//用于接管BIOS内存统计的结果
typedef struct {
    uint    addr_start;     // 可用内存起始地址 一般是1M
    uint    addr_end;       // 可用内存结束地址
    uint    valid_mem_size;
    uint    pages_total;    // 机器物理内存共多少page
    uint    pages_free;     // 机器物理内存还剩多少page
    uint    pages_used;     // 机器物理内存用了多少page
}physics_memory_info_t;

//
typedef struct {
    uint            addr_base;          // 可用物理内存开始位置  3M
    uint            pages_total;        // 共有多少page   机器物理内存共多少page - 0x30000（3M）
    uint            bitmap_item_used;  // 如果1B映射一个page，用了多少个page
    uchar*          map;
}physics_memory_map_t;

//打印BIOS内存检测结果
void print_check_memory_info();
//初始化内存，统计可用内存起始地址，以及可以分多少页
void memory_init();
//初始化内存位图
void memory_map_int();
//初始化虚拟内存
void virtual_memory_init();
//获得空闲页表，分配、释放物理内存
void* get_free_page();
void free_page(void* p);

//分配、释放虚拟内存
void* malloc(size_t size);
void free_s(void *obj, int size);

#endif //ZIYA_OSKERNEL_TEACH_MM_H


#endif //OS_MM_H
