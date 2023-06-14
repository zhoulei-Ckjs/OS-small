#include "../../include/linux/kernel.h"
#include "../../include/linux/traps.h"

//时钟中断处理函数
void clock_handler(int idt_index) {
    //send_eoi(idt_index);

    printf("0x%x\n", idt_index);
}