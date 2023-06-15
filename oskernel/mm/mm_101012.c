#include "../include/asm/system.h"
#include "../include/linux/kernel.h"
#include "../include/linux/mm.h"
#include "../include/string.h"

//初始化虚拟内存
void virtual_memory_init() {
    int* pdt = (int*)get_free_page();       //建立页表项，一个页表4B，因此声明为int*

    // 清零
    memset(pdt, 0, PAGE_SIZE);

    //初始化4个页表
    for (int i = 0; i < 4; ++i) {
        // pdt里面的每项，即pde，内容是ptt + 尾12位的权限位
        int ptt = (int)get_free_page() + (i * 0x1000);
        int pde = 0b00000000000000000000000000000111 | ptt;     //普通用户访问_可读写_有效

        pdt[i] = pde;

        int* ptt_arr = (int*)ptt;

        if (0 == i) {
            // 第一块映射区，给内核用
            for (int j = 0; j < 0x400; ++j) {       //一个page有1024个页表项
                int* item = &ptt_arr[j];

                int virtual_addr = j * 0x1000;
                *item = 0b00000000000000000000000000000111 | virtual_addr;  //初始化二级页表中的内容
            }
        } else {
            for (int j = 0; j < 0x400; ++j) {
                int* item = &ptt_arr[j];

                int virtual_addr = j * 0x1000;
                virtual_addr = virtual_addr + i * 0x400 * 0x1000;   //给内核分配了1024个page，故偏移i × 1024个 × 4096（页大小）

                *item = 0b00000000000000000000000000000111 | virtual_addr;      //初始化二级页表中的内容
            }
        }
    }

    BOCHS_DEBUG_MAGIC

    set_cr3((uint)pdt);

    enable_page();

    BOCHS_DEBUG_MAGIC
}