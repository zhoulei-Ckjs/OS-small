[bits 32]
[SECTION .text]

extern printf

;中断处理程序
global interrupt_handler
interrupt_handler:
    push msg
    call printf
    add esp, 4              ;平栈

    iret

msg:
    db "interrupt_handler", 10, 0