[bits 32]
[SECTION .text]

extern printf
extern keymap_handler

;中断处理程序
global interrupt_handler
interrupt_handler:
    push msg
    call printf
    add esp, 4              ;平栈

    iret

; 键盘中断
global keymap_handler_entry
keymap_handler_entry:
    push 0x21
    call keymap_handler     //读取字符，对字符进行接收
    add esp, 4

    iret

msg:
    db "interrupt_handler", 10, 0