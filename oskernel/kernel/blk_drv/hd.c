#include "../../include/linux/hd.h"
#include "../../include/linux/kernel.h"
#include "../../include/asm/io.h"

// BIOS例程会将检测到的硬盘数量写在内存0x475位置（注意：0x475为物理地址，如果开启分页映射方式不对可能读不到）
#define HD_NUMBER_MEMORY_PTR 0x475

static void _hd_init()
{
    /**
     * 取到当前挂的硬盘数量
     * 第1、2块盘对应的是通道1的主从盘，第3、4块盘对应的是通道2的主从盘。第5块盘qemu就不支持了
     */
    u8 hd_number = *(u8*)HD_NUMBER_MEMORY_PTR;
    printk("disk number: %d\n", hd_number);
}

void hd_init()
{
    printk("hd init...\n");
    _hd_init();
}

void hd_drive()
{
    char buf[512] = {0};
//    short v = in_word(0x1f0);
    port_read(0x1f0, buf, 255);     //从磁盘读取一个扇区
    printk("123456\n");         //一个扇区后面的55 aa应该没了，因为就读取了255次循环，而读完一个扇区需要256次
}