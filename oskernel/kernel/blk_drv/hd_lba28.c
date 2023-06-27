#include "../../include/linux/hd.h"

// 用于保存当前硬盘下一次发生中断该如何处理的函数，如读命令，发生中断后这里应该保存读函数
dev_handler_fun_t dev_interrupt_handler;


void do_identify()
{
    printk("[%s:%d]run...\n", __FUNCTION__, __LINE__);

    char buf[512] = {0};
    // 读硬盘
    port_read(0x1f0, buf, 256);

    /*
     * 如何解析再说
     * */
    printk("123456\n");         //临时打印数字
}

/**
 * 发送相应的读写命令
 * @param hd
 * @param from
 * @param count
 * @param cmd
 * @param handler
 */
void hd_out(char hd, int from, int count, unsigned int cmd, dev_handler_fun_t handler)
{
    // 这个得放在向硬盘发起请求的前面，否则中断例程中用的时候是没值的
    dev_interrupt_handler = handler;

    out_byte(HD_NSECTOR, count);
    out_byte(HD_SECTOR, from & 0xff);
    out_byte(HD_LCYL, from >> 8 & 0xff);
    out_byte(HD_HCYL, from >> 16 & 0xff);
    out_byte(HD_CURRENT, 0b11100000 | (hd << 4) | (from >> 24 & 0xf));
    out_byte(HD_COMMAND, cmd);
}

