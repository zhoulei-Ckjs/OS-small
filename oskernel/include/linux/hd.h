#ifndef OS_HD_H
#define OS_HD_H

/**
 * 从端口读取数据到缓冲区
 * cld  清除方向标志位，确保数据按照顺序被读入缓冲区中
 * rep  重复ecx次数
 * insw 从I/O端口读取一个字（16位）的数据，并存储到ES:DI寄存器指向的内存地址中，重复执行直到读取的字数达到cx寄存器中的值
 * "d" 对应于 DX 寄存器，用于存储 I/O 端口地址。
 * "D" 对应于 EDI 寄存器，用于存储数据缓冲区的地址。
 */
#define port_read(port, buf, nr) \
__asm__("cld;rep;insw"::"d" (port),"D" (buf),"c" (nr))

/**
 * 从端口读取数据到缓冲区
 * cld  清除方向标志位，确保数据按照顺序被读入缓冲区中
 * rep  重复ecx次数
 * outsw 通过反复执行 "outsw" 指令，可以将缓冲区中的所有数据都写入到目标端口中。
 * "d" 对应于 DX 寄存器，用于存储 I/O 端口地址。
 * "S" 表示寄存器 "ESI"
 */
#define port_write(port, buf, nr) \
__asm__("cld;rep;outsw"::"d" (port),"S" (buf),"c" (nr))

/**
 * 初始化硬盘
 */
void hd_init();

#endif //OS_HD_H
