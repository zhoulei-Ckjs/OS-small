#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"
#include "../include/linux/traps.h"
#include "../include/linux/mm.h"
#include "../include/linux/task.h"
#include "../include/linux/sched.h"

extern void clock_init();

void kernel_main(void)
{
    console_init();     //初始化控制台，清空屏幕
    gdt_init();         //初始化gdt全局描述符表
    idt_init();         //初始化中断描符表
    clock_init();       //时钟中断初始化，设定10ms一次中断，可以到clock.c中设置

    print_check_memory_info();      //内存的检测结果
    memory_init();                  //初始化内存
    memory_map_int();               //对内存进行位图管理

    //virtual_memory_init();          //虚拟内存
    task_init();
    sched();                        //任务调度


    // 测试分配虚拟内存
    void* p = malloc(1);
    free_s(p, 1);
    p = malloc(1);
    free_s(p, 1);
    p = malloc(2);
    free_s(p, 2);
    p = malloc(4096);
    free_s(p, 4096);

    __asm__("sti;");    //打开中断

    //除零异常，引发软件中断
    //int i = 10 / 0;
    while (true);
}
