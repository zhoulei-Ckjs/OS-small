#include "../include/asm/system.h"
#include "../include/linux/head.h"
#include "../include/linux/traps.h"
#include "../include/linux/kernel.h"

#define INTERRUPT_TABLE_SIZE    256

interrupt_gate_t interrupt_table[INTERRUPT_TABLE_SIZE] = {0};

//char idt_ptr[6] = {0};
/*
 * 中断向量表指针
 */
xdt_ptr_t idt_ptr;

// 普通中断处理程序
extern void interrupt_handler_entry();
// 键盘中断处理程序
extern void keymap_handler_entry();
/*
 * 时钟中断处理程序入口
 * 定义在clock_handler.asm中
 */
extern void clock_handler_entry();
// 在interrupt_handler.asm中定义，系统调用入口
extern void system_call_entry();
/**
 * 硬盘中断处理程序
 */
extern void hd_handler_entry();

// 是在汇编中定义的中断处理程序，0-19的系统保留中断
extern int interrupt_handler_table[0x2f];

//初始化中断向量表
void idt_init()
{
    printk("init idt...\n");
    CLI
    for (int i = 0; i < INTERRUPT_TABLE_SIZE; ++i)
    {
        interrupt_gate_t* p = &interrupt_table[i];

        int handler = (int)interrupt_handler_entry;

        /*
         * 中断处理程序，其中0-19，是特定的异常与错误；20-31，映射中断控制芯片的IRQ0-IRQ15
         * */
        if (i <= 0x15) {
            handler = (int)interrupt_handler_table[i];
        }

        /*
         * 时钟中断，32号中断
         * 在x86架构的计算机中，时钟中断的中断号为0x20，这个值是由Intel设计的8259A可编程中断控制器（PIC）所规定的。
         * */
        if (0x20 == i)
        {
            handler = (int)clock_handler_entry;
        }

        /**
         * 硬盘中断处理程序，注册到中断向量表
         */
        if (0x2e == i) {
            handler = (int)hd_handler_entry;
        }

        /*
         * 调用门中断处理函数，用于处理用户态的系统调用
         */
        if (0x80 == i)
        {
            handler = (int)system_call_entry;
        }

        /*
         * 键盘中断程序,定义为33号中断
         * */
        if (0x21 == i) {
            handler = (int)keymap_handler_entry;
        }

        p->offset0 = handler & 0xffff;
        p->offset1 = (handler >> 16) & 0xffff;
        p->selector = 1 << 3; // 代码段选择子
        p->reserved = 0;      // 保留不用
        p->type = 0b1110;     // 中断门
        p->segment = 0;       // 系统段，段基址是0，保护模式利用段基址+选择子进行访问
        //p->DPL = 0;           // 内核态，ring0级别
        p->DPL = (0x80 == i)? 3 : 0;    // 内核态，ring0级别，中断描述符为ring3
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