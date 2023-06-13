/*
 * io.asm 的头文件
 * */

#ifndef OS_IO_H
#define OS_IO_H

char in_byte(int port);
void out_byte(int port, int v);

short in_word(int port);
void out_word(int port, int v);

#endif //OS_IO_H
