#ifndef OS_HEAD_H
#define OS_HEAD_H

/*
 * gdt结构体
 * */
typedef struct gdt_item_t {
    unsigned short limit_low;      // 段界限 0 ~ 15 位
    unsigned int base_low : 24;    // 基地址 0 ~ 23 位 16M
    unsigned char type : 4;        // 段类型
    unsigned char segment : 1;     // 1 表示代码段或数据段，0 表示系统段
    unsigned char DPL : 2;         // Descriptor Privilege Level 描述符特权等级 0 ~ 3
    unsigned char present : 1;     // 存在位，1 在内存中，0 在磁盘上
    unsigned char limit_high : 4;  // 段界限 16 ~ 19;
    unsigned char available : 1;   // 该安排的都安排了，送给操作系统吧
    unsigned char long_mode : 1;   // 64 位扩展标志
    unsigned char big : 1;         // 32 位 还是 16 位;
    unsigned char granularity : 1; // 粒度 4KB 或 1B
    unsigned char base_high;       // 基地址 24 ~ 31 位
} __attribute__((packed)) gdt_item_t;
//__attribute__((packed)) GCC编译器的一个扩展功能，它告诉编译器在编译结构体时不要进行对齐优化

//段选择子
typedef struct gdt_selector_t {
    char RPL : 2;                   //请求特权级
    char TI : 1;                    //0表示gdt，1表示ldt
    short index : 13;               //gdt或ldt数组的索引号
} __attribute__((packed)) gdt_selector_t;

//用于存储gdt的基地址和大小
#pragma pack(2)
typedef struct xdt_ptr_t {
    short   limit;      // gdt表大小
    int     base;       // 基地址，这个base需要大端存储，默认是小端存储
} xdt_ptr_t;
#pragma pack()


/*
 * 中断门的定义
 * */
typedef struct interrupt_gate_t {
    short offset0;    // 段内偏移 0 ~ 15 位
    short selector;   // 代码段选择子
    char reserved;    // 保留不用
    char type : 4;    // 任务门/中断门/陷阱门
    char segment : 1; // segment = 0 表示系统段
    char DPL : 2;     // 使用 int 指令访问的最低权限
    char present : 1; // 是否有效
    short offset1;    // 段内偏移 16 ~ 31 位
} __attribute__((packed)) interrupt_gate_t;



#endif //OS_HEAD_H
