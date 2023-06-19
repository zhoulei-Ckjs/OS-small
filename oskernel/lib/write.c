#include "../include/unistd.h"

/**
 * 根据宏定义unistd.h生成一个write函数
 * @return
 */
_syscall3(int, write, int, fd, const char *, buf, int, count)
/**
int write(int fd, const char * buf, int count)
{
    long __res;
    __asm__ volatile("int 0x80" : "=a"(__res) : "0"(0),"b" ((long)fd),"c"((long)buf),"d"((long)count));
        "=a"(__res) 表示将返回值给__res
        "0"(0)表示，第一个寄存器为eax，将其赋值为0
        ebx = fd
        ecx = buf
        edx = count
    if(__res >= 0)
        return (int)__res;
    errno = -__res;
    return -1;
}
 */