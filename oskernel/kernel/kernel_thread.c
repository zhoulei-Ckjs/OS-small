#include "../include/linux/hd.h"
#include "../include/linux/kernel.h"

void kernel_thread_fun(void* arg)
{
    hd_init();

    char hd = 0;
    int from = 0;
    int count = 1;
//    unsigned int cmd = 0xec;        //检测硬盘
    unsigned int cmd = 0x20;        //读硬盘
//    unsigned int cmd = 0x30;        //写硬盘

/*
 * 现在有个问题，在多任务情况下，一个进程要读取硬盘，但是在时钟中断情况下，可能当前执行的是另外一个进程
 * 这个时候如果发生中断，那么就是另外一个进程在执行
 */
    hd_out(hd, from, count, cmd, hd_drive);         //读磁盘
//    hd_out(hd, from, count, cmd, do_identify);      //检测磁盘

    while(true)
    {
        asm volatile("sti; hlt");   //STI用于设置处理器的中断标志位，允许中断。而HLT用于使处理器进入等待状态，直到收到中断信号后才会唤醒。
    }
}