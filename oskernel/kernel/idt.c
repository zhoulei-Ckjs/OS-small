#include "../include/asm/system.h"
#include "../include/linux/head.h"
#include "../include/linux/traps.h"
#include "../include/linux/kernel.h"

#define INTERRUPT_TABLE_SIZE    256

interrupt_gate_t interrupt_table[INTERRUPT_TABLE_SIZE] = {0};

//char idt_ptr[6] = {0};
xdt_ptr_t idt_ptr;

// 普通中断处理程序
extern void interrupt_handler_entry();
// 键盘中断处理程序
extern void keymap_handler_entry();

// 是在汇编中定义的中断处理程序
extern int interrupt_handler_table[0x2f];

//初始化中断向量表
void idt_init() {
    printf("init idt...\n");
    for (int i = 0; i < INTERRUPT_TABLE_SIZE; ++i) {
        interrupt_gate_t* p = &interrupt_table[i];

        int handler = (int)interrupt_handler_entry;

        /*
         * 中断处理程序，其中0-19，是特定的异常与错误；20-31，映射中断控制芯片的IRQ0-IRQ15
         * */
        if (i <= 0x13) {        //系统保留的中断号，这里我认为是0x13
            handler = (int)interrupt_handler_table[i];
        }
        /*
         * 键盘中断程序,定义为33号中断
         * */
        if (0x21 == i) {
            handler = (int)keymap_handler_entry;
        }

        p->offset0 = handler & 0xffff;
        p->offset1 = (handler >> 16) & 0xffff;
        p->selector = 1 << 3; // 代码段
        p->reserved = 0;      // 保留不用
        p->type = 0b1110;     // 中断门
        p->segment = 0;       // 系统段
        p->DPL = 0;           // 内核态
        p->present = 1;       // 有效
    }

    // 让CPU知道中断向量表
    write_xdt_ptr(&idt_ptr, INTERRUPT_TABLE_SIZE * 8, interrupt_table);
    //*(short*)idt_ptr = INTERRUPT_TABLE_SIZE * 8;    //2个字节存储idt表大小
    //*(int*)(idt_ptr + 2) = interrupt_table;         //4个字节存idt表结构体指针

    //BOCHS_DEBUG_MAGIC

    asm volatile("lidt idt_ptr;");
    //BOCHS_DEBUG_MAGIC   //查看idt表是否更换
}