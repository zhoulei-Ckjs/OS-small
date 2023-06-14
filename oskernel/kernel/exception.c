#include "../include/linux/kernel.h"
#include "../include/linux/types.h"

char *messages[] = {
        "#DE Divide Error\0",
        "#DB RESERVED\0",
        "--  NMI Interrupt\0",
        "#BP Breakpoint\0",
        "#OF Overflow\0",
        "#BR BOUND Range Exceeded\0",
        "#UD Invalid Opcode (Undefined Opcode)\0",
        "#NM Device Not Available (No Math Coprocessor)\0",
        "#DF Double Fault\0",
        "    Coprocessor Segment Overrun (reserved)\0",
        "#TS Invalid TSS\0",
        "#NP Segment Not Present\0",
        "#SS Stack-Segment Fault\0",
        "#GP General Protection\0",
        "#PF Page Fault\0",
        "--  (Intel reserved. Do not use.)\0",
        "#MF x87 FPU Floating-Point Error (Math Fault)\0",
        "#AC Alignment Check\0",
        "#MC Machine Check\0",
        "#XF SIMD Floating-Point Exception\0",
        "#VE Virtualization Exception\0",
        "#CP Control Protection Exception\0",
};

void div_zero_exception(int idt_index) {
    char* msg = messages[idt_index];
    printf("%s\n", msg);
}

// 13 一般性保护异常
void normal_protect_exception(int idt_index) {
    printf("%s\n", messages[idt_index]);
}

//打印中断信息
//idt_index是中断索引
void exception_handler(int idt_index, int edi, int esi, int ebp, int esp,
                       int ebx, int edx, int ecx, int eax, int eip, char cs, int eflags) {
    printf("\n==========\n");
    printf("EXCEPTION : %s \n", messages[idt_index]);
    printf("   VECTOR : 0x%02X\n", idt_index);
    printf("   EFLAGS : 0x%08X\n", eflags);
    printf("       CS : 0x%02X\n", cs);
    printf("      EIP : 0x%08X\n", eip);
    printf("      ESP : 0x%08X\n", esp);
    printf("==========\n");

    while (true);
}