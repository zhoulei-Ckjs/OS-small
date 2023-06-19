#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/linux/traps.h"
#include "../include/linux/mm.h"
#include "../include/linux/task.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"

extern void clock_init();

void user_mode() {
    int age = 10;

    char* str = "welcome";
    printf("%s, %d\n", str, 11);
}

void kernel_main(void)
{
    console_init();     //初始化控制台，清空屏幕
    gdt_init();         //初始化gdt全局描述符表
    idt_init();         //初始化中断向量表
    clock_init();       //时钟中断初始化，设定10ms一次中断，可以到clock.c中设置

    print_check_memory_info();      //内存的检测结果
    memory_init();                  //初始化内存
    memory_map_int();               //对内存进行位图管理

    task_init();
    //sched();                        //任务调度

    __asm__("sti;");    //打开中断

    user_mode();

    while (true);
}
