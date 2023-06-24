#include "../include/linux/hd.h"
#include "../include/linux/kernel.h"

#define HD_DATA		0x1f0	/* _CTL when writing */
#define HD_ERROR	0x1f1	/* see err-bits */
#define HD_NSECTOR	0x1f2	/* nr of sectors to read/write */
#define HD_SECTOR	0x1f3	/* starting sector */
#define HD_LCYL		0x1f4	/* starting cylinder */
#define HD_HCYL		0x1f5	/* high byte of starting cyl */
#define HD_CURRENT	0x1f6	/* 101dhhhh , d=drive, hhhh=head */
#define HD_STATUS	0x1f7	/* see status-bits */
#define HD_PRECOMP HD_ERROR	/* same io address, read=error, write=precomp */
#define HD_COMMAND HD_STATUS	/* same io address, read=status, write=cmd */

void kernel_thread_fun(void* arg)
{
    hd_init();

    char hd = 0;
    int from = 0;
    int count = 1;
    unsigned int cmd = 0xec;        //检测硬盘
//    unsigned int cmd = 0x20;        //读硬盘
//    unsigned int cmd = 0x30;        //写硬盘


    out_byte(HD_NSECTOR, count);
    out_byte(HD_SECTOR, from & 0xff);
    out_byte(HD_LCYL, from >> 8 & 0xff);
    out_byte(HD_HCYL, from >> 16 & 0xff);
    out_byte(HD_CURRENT, 0b11100000 | (hd << 4) | (from >> 24 & 0xf));
    out_byte(HD_COMMAND, cmd);

    while(true)
    {
        asm volatile("sti; hlt");   //STI用于设置处理器的中断标志位，允许中断。而HLT用于使处理器进入等待状态，直到收到中断信号后才会唤醒。
    }
}