#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/linux/traps.h"

extern void clock_init();

void kernel_main(void)
{
    console_init();     //初始化控制台，清空屏幕
    gdt_init();         //初始化gdt全局描述符表
    idt_init();         //初始化中断描符表
    clock_init();       //时钟中断初始化，设定10ms一次中断，可以到clock.c中设置

    printf("-------kernel_main-------\n");

    __asm__("sti;");    //打开中断

    //除零异常，引发软件中断
    //int i = 10 / 0;

    while (true);

}
