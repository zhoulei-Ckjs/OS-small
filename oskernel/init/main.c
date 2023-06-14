#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/linux/traps.h"

void kernel_main(void)
{
    console_init();     //初始化控制台，清空屏幕
    gdt_init();         //初始化gdt全局描述符表
    idt_init();         //初始化中断描符表

    printf("-------kernel_main-------\n");

    __asm__("sti;");    //打开中断

    while (true);

}
