[BITS 32]
[SECTION .text]

; fork如果用c实现，就会因为栈问题导致出错
global fork
fork:
    mov eax, 2
    int 0x80        ; 0x80中断会调用system_call_table，在idt.c中入口
                    ; 中断会进入内核态，ss变为0x10就是内核态代码段选择子

    ret