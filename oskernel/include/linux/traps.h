#ifndef OS_TRAPS_H
#define OS_TRAPS_H
/*
 * 故障或称为陷阱
 * */
#include "head.h"
void gdt_init();            //初始化gdt表
void idt_init();            //初始化idt表（中断向量表）

#endif //OS_TRAPS_H
