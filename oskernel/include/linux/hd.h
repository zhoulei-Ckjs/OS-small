#ifndef OS_HD_H
#define OS_HD_H

#include "types.h"

#define HD_NSECTOR	0x1f2	//往这个端口发送要读/写多少个扇区
#define HD_SECTOR	0x1f3	//这个端口发送数据表示从哪个扇区开始读/写
#define HD_LCYL		0x1f4	//这个端口发送的数据能够指定所要读/写的柱面（低字节）
#define HD_HCYL		0x1f5	//这个端口发送的数据能够指定所要读/写的柱面（高字节）
#define HD_CURRENT	0x1f6	//在该寄存器中，高四位（hhhh）表示磁头（head）号，低三位（ddd）表示磁盘驱动器（drive）号，其余位保留
#define HD_STATUS	0x1f7	//端口描述了硬盘状态，也用于下达命令
#define HD_COMMAND HD_STATUS	//读时作为状态，写时作为命令

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
 * "c" ecx的值为nr，表示要读取多少次循环，一次循环2个字节，512字节需要256次则nr值需要传递为256
 */
#define port_write(port, buf, nr) \
__asm__("cld;rep;outsw"::"d" (port),"S" (buf),"c" (nr))

/**
 * 定义处理函数类型
 */
typedef void (*dev_handler_fun_t)(void);

/**
 * 初始化硬盘
 */
void hd_init();

/**
 * 发送命令
 * @param hd
 * @param from
 * @param count
 * @param cmd
 * @param handler
 */
void hd_out(char hd, int from, int count, unsigned int cmd, dev_handler_fun_t handler);

void hd_drive();
/**
 * 用于检测磁盘
 */
void do_identify();

#endif //OS_HD_H
