#ifndef OS_TRAPS_H
#define OS_TRAPS_H
/*
 * 故障或称为陷阱
 * */
#include "head.h"
void gdt_init();            //初始化gdt表
void idt_init();            //初始化idt表（中断向量表）

void send_eoi(int idt_index);   //end of interrupt
void write_xdt_ptr(xdt_ptr_t* p, short limit, int base);

#endif //OS_TRAPS_H
