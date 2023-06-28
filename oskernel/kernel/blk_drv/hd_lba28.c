#include "../../include/linux/hd.h"

// 用于保存当前硬盘下一次发生中断该如何处理的函数，如读命令，发生中断后这里应该保存读函数
dev_handler_fun_t dev_interrupt_handler;


/**
 * 获取发送硬盘操作请求后硬盘的状态
 *  准备就绪返回0
 *  出差返回错误寄存器中存储的值
 * @return 0表示没有问题，其他返回错误码
 */
static int win_result()
{
    int i = in_byte(HD_STATUS);         //从端口读取硬盘状态
    printk("[%s:%d]hd status: 0x%x\n", __FUNCTION__, __LINE__, i);

    if ((i & (BUSY_STAT | READY_STAT | WRERR_STAT | SEEK_STAT | ERR_STAT)) == (READY_STAT | SEEK_STAT))
        return 0; /* ok */
    if (i & 1)
    {
        i = in_byte(HD_ERROR);
    }
    return i;
}

void do_identify()
{
    printk("[%s:%d]run...\n", __FUNCTION__, __LINE__);

    int status = 0;
    if(0 != (status = win_result()))            //获取硬盘状态
    {
        printk("identify disk error: %d\n", status);
        return;
    }

    char buf[512] = {0};
    // 读硬盘
    port_read(0x1f0, buf, 256);

    hd_t hd;
    // 从identify返回结果中取出硬盘信息
    memcpy(&hd.number, buf + 10 * 2, 10 * 2);
    hd.number[21] = '\0';
    memcpy(&hd.model, buf + 27 * 2, 20 * 2);
    hd.model[41] = '\0';

    hd.sectors = *(int*)(buf + 60 * 2);

    print_disk_info(&hd);

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

void print_disk_info(hd_t* info)
{
    printk("===== Hard Disk Info Start =====\n");
    printk("Hard disk Serial number: %s\n", info->dev_no);
    printk("Drive model: %s\n", info->model);
    printk("Hard disk size: %d sectors, %d M\n", info->sectors, info->sectors * 512 / 1024 / 1024);
    printk("===== Hard Disk Info End =====\n");
}