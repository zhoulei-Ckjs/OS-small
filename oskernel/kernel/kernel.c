#include "../include/linux/kernel.h"

inline uint get_cr3() {
    asm volatile("mov eax, cr3;");
}

inline void set_cr3(uint v)
{
    asm volatile("mov cr3, eax;" ::"a"(v));     //a表示eax寄存器，将v给eax，然后给cr3
}

/*
 * 开启101012分页直接加载页表就行，开启29912分页跟cr4寄存器有关
 */
inline void enable_page() {
    asm volatile("mov eax, cr0;"
                 "or eax, 0x80000000;"      //将最高位置1表示开启分页
                 "mov cr0, eax;");
}