/*
 * 声明两个函数，初始化console和打印字符串
 * */

#ifndef OS_TTY_H
#define OS_TTY_H

#include "types.h"

void console_init(void);
/**
 * 系统控制台写字符
 * @param buf
 * @param count
 * @return
 */
int console_write(char *buf, u32 count);

#endif //OS_TTY_H
