#include "../include/asm/system.h"
#include "../include/linux/kernel.h"
#include "../include/linux/mm.h"
#include "../include/string.h"
#include "../include/linux/task.h"

// 页表从0x20000开始存
#define PDT_START_ADDR 0x20000

//初始化虚拟内存
void *virtual_memory_init()
{
//    int* pdt = (int*)get_free_page();       //建立页表项，一个页表4B，因此声明为int*
    int* pdt = (int*)PDT_START_ADDR;

    // 清零
    memset(pdt, 0, PAGE_SIZE);

    //初始化4个页表
    for (int i = 0; i < 4; ++i)
    {
        // pdt里面的每项，即pde，内容是ptt + 尾12位的权限位
        int ptt = (int)PDT_START_ADDR + ((i + 1) * 0x1000);     //这里的+1是前面的4096字节作为pdt表
        int pde = 0b00000000000000000000000000000111 | ptt;     //普通用户访问_可读写_有效

        pdt[i] = pde;       //这里循环4次，说明pdt表中初始化了4个pde

        int* ptt_arr = (int*)ptt;

        //初始化刚建立的4个二级页表
        if (0 == i)
        {
            // 第一块映射区，给内核用
            for (int j = 0; j < 0x400; ++j)         //一个page有1024个页表项（一个页表4B）
            {
                int* item = &ptt_arr[j];

                int virtual_addr = j * 0x1000;      //初始化第12位为1（从0开始，暂时不知道干嘛的）
                *item = 0b00000000000000000000000000000111 | virtual_addr;  //初始化二级页表中的内容
                    //普通用户也可以访问_可读写_有效
            }
        }
        else
        {
//            for (int j = 0; j < 0x400; ++j) {
//                int* item = &ptt_arr[j];
//
//                int virtual_addr = j * 0x1000;
//                virtual_addr = virtual_addr + i * 0x400 * 0x1000;   //给内核分配了1024个page，故偏移i × 1024个 × 4096（页大小）
//
//                *item = 0b00000000000000000000000000000111 | virtual_addr;      //初始化二级页表中的内容
//            }
        }
    }

    //BOCHS_DEBUG_MAGIC

    set_cr3((uint)pdt);

    enable_page();

    //BOCHS_DEBUG_MAGIC

    printk("pdt addr: 0x%p\n", pdt);

    return pdt;
}