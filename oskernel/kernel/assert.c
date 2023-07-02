#include "../include/linux/kernel.h"
static u8 buf[1024];

// 强制阻塞
static void spin(char *name)
{
    printk("spinning in %s ...\n", name);
    while (true);
}

void panic(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int i = vsprintf(buf, fmt, args);
    va_end(args);

    printk("!!! panic !!!\n--> %s \n", buf);
    spin("panic()");

    // 不可能走到这里，否则出错；"ud2" 是一条 x86 汇编语言指令，是 "undefined instruction" 的缩写，也就是未定义指令
    // 这个指令会引发一个 "invalid opcode" 的异常
    asm volatile("ud2");
}
