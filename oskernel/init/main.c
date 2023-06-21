#include "../include/linux/tty.h"
#include "../include/linux/traps.h"
#include "../include/linux/mm.h"
#include "../include/linux/task.h"
#include "../include/stdio.h"
#include "../include/unistd.h"


extern void clock_init();

void user_mode()
{
    char* str = "welcome";
    printf("%s, %d\n", str, 11);

//    pid_t pid = fork();
//    if (pid > 0) {
//        printf("pid=%d, ppid=%d\n", getpid(), getppid());
//    } else if (0 == pid) {
//        printf("pid=%d, ppid=%d\n", getpid(), getppid());
//
//        for (int i = 0; i < 10; ++i) {
//            printf("%d\n", i);
//        }
////        while (true);
//    }
}

void kernel_main(void)
{
    console_init();     //初始化控制台，清空屏幕
    clock_init();       //时钟中断初始化，设定10ms一次中断，可以到clock.c中设置

    print_check_memory_info();      //内存的检测结果
    memory_init();                  //初始化内存
    memory_map_int();               //对内存进行位图管理

    gdt_init();         //初始化gdt全局描述符表
    idt_init();         //初始化中断向量表

    task_init();

    __asm__("sti;");    //打开中断

    while (true);
}
