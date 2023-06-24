[bits 32]
[SECTION .text]

extern printk
extern keymap_handler       ;中断处理程序
extern exception_handler    ;异常处理程序
extern system_call_table    ;系统调用列表，system_call.c中
extern system_call
extern hd_drive             ;硬盘驱动

extern current              ;当前任务

;中断处理程序
global interrupt_handler_entry
interrupt_handler_entry:
    push msg
    call printk
    add esp, 4              ;平栈

    iret

;硬盘中断处理程序
global hd_handler_entry
hd_handler_entry:
    call hd_drive
    iret

; 键盘中断
global keymap_handler_entry
keymap_handler_entry:
    push 0x21
    call keymap_handler     ;读取字符，对字符进行接收
    add esp, 4

    iret

; 系统调用中断入口
; eax = 调用号
; ebx = 第一个参数
; ecx = 第二个参数
; edx = 第三个参数
global system_call_entry
system_call_entry:
    ; 这里ss由0x2b变为0x10，esp由0x106000变为0x101fec，为什么？
    ; 理由是当cpu发生中断时会切换到内核的栈空间，会去tss段中寻找esp0的值，这个地址我们在 init_tss_item() 时初始化的。
    ; 同时进入内核态后会将ss改为内核态内核态的代码段选择子
    mov esi, [current]

    mov edi, [esp + 4 * 3]          ; 在x86架构上，当CPU执行系统调用时，会自动将当前用户态的程序现场保存到内核栈中。
    mov [esi + 4 * 14], edi         ; 保存r3 esp

    mov [esi + 4 * 15], ebp

    push edx
    push ecx
    push ebx

    call [system_call_table + eax * 4]

    ; 恢复esp,前面压入了三个参数
    add esp, 12

    ; 恢复ebp
    mov esi, [current]
    mov ebp, [esi + 4 * 15]

    xchg bx, bx

    iret


; 异常处理
;
; eflags                |
; cs                    |
; eip                   |
; eax                   |
; ecx                栈在内存
; edx                   |
; ebx                   |
; esp                   |
; ebp                   |
; esi                   |
; edi                 \ | /
; idt_index            \ /

%macro INTERRUPT_HANDLER 1
global interrupt_handler_%1
interrupt_handler_%1:
    pushad                      ;x86保存全部寄存器

    push %1
    call exception_handler
    add esp, 4

    popad

    iret
%endmacro

INTERRUPT_HANDLER 0x00; divide by zero
INTERRUPT_HANDLER 0x01; debug
INTERRUPT_HANDLER 0x02; non maskable interrupt
INTERRUPT_HANDLER 0x03; breakpoint

INTERRUPT_HANDLER 0x04; overflow
INTERRUPT_HANDLER 0x05; bound range exceeded
INTERRUPT_HANDLER 0x06; invalid opcode
INTERRUPT_HANDLER 0x07; device not avilable

INTERRUPT_HANDLER 0x08; double fault
INTERRUPT_HANDLER 0x09; coprocessor segment overrun
INTERRUPT_HANDLER 0x0a; invalid TSS
INTERRUPT_HANDLER 0x0b; segment not present

INTERRUPT_HANDLER 0x0c; stack segment fault
INTERRUPT_HANDLER 0x0d; general protection fault
INTERRUPT_HANDLER 0x0e; page fault
INTERRUPT_HANDLER 0x0f; reserved

INTERRUPT_HANDLER 0x10; x87 floating point exception
INTERRUPT_HANDLER 0x11; alignment check
INTERRUPT_HANDLER 0x12; machine check
INTERRUPT_HANDLER 0x13; SIMD Floating - Point Exception

INTERRUPT_HANDLER 0x14; Virtualization Exception
INTERRUPT_HANDLER 0x15; Control Protection Exception

INTERRUPT_HANDLER 0x16; reserved
INTERRUPT_HANDLER 0x17; reserved
INTERRUPT_HANDLER 0x18; reserved
INTERRUPT_HANDLER 0x19; reserved
INTERRUPT_HANDLER 0x1a; reserved
INTERRUPT_HANDLER 0x1b; reserved
INTERRUPT_HANDLER 0x1c; reserved
INTERRUPT_HANDLER 0x1d; reserved
INTERRUPT_HANDLER 0x1e; reserved
INTERRUPT_HANDLER 0x1f; reserved

INTERRUPT_HANDLER 0x20; clock 时钟中断
INTERRUPT_HANDLER 0x21; 键盘中断
INTERRUPT_HANDLER 0x22
INTERRUPT_HANDLER 0x23
INTERRUPT_HANDLER 0x24
INTERRUPT_HANDLER 0x25
INTERRUPT_HANDLER 0x26
INTERRUPT_HANDLER 0x27
INTERRUPT_HANDLER 0x28; rtc 实时时钟
INTERRUPT_HANDLER 0x29
INTERRUPT_HANDLER 0x2a
INTERRUPT_HANDLER 0x2b
INTERRUPT_HANDLER 0x2c
INTERRUPT_HANDLER 0x2d
INTERRUPT_HANDLER 0x2e
INTERRUPT_HANDLER 0x2f


; 中断向量表
global interrupt_handler_table
interrupt_handler_table:
    dd interrupt_handler_0x00       ; 中断向量表地址，double word 4字节
    dd interrupt_handler_0x01
    dd interrupt_handler_0x02
    dd interrupt_handler_0x03
    dd interrupt_handler_0x04
    dd interrupt_handler_0x05
    dd interrupt_handler_0x06
    dd interrupt_handler_0x07
    dd interrupt_handler_0x08
    dd interrupt_handler_0x09
    dd interrupt_handler_0x0a
    dd interrupt_handler_0x0b
    dd interrupt_handler_0x0c
    dd interrupt_handler_0x0d
    dd interrupt_handler_0x0e
    dd interrupt_handler_0x0f
    dd interrupt_handler_0x10
    dd interrupt_handler_0x11
    dd interrupt_handler_0x12
    dd interrupt_handler_0x13
    dd interrupt_handler_0x14
    dd interrupt_handler_0x15
    dd interrupt_handler_0x16
    dd interrupt_handler_0x17
    dd interrupt_handler_0x18
    dd interrupt_handler_0x19
    dd interrupt_handler_0x1a
    dd interrupt_handler_0x1b
    dd interrupt_handler_0x1c
    dd interrupt_handler_0x1d
    dd interrupt_handler_0x1e
    dd interrupt_handler_0x1f
    dd interrupt_handler_0x20
    dd interrupt_handler_0x21
    dd interrupt_handler_0x22
    dd interrupt_handler_0x23
    dd interrupt_handler_0x24
    dd interrupt_handler_0x25
    dd interrupt_handler_0x26
    dd interrupt_handler_0x27
    dd interrupt_handler_0x28
    dd interrupt_handler_0x29
    dd interrupt_handler_0x2a
    dd interrupt_handler_0x2b
    dd interrupt_handler_0x2c
    dd interrupt_handler_0x2d
    dd interrupt_handler_0x2e
    dd interrupt_handler_0x2f

msg:
    db "interrupt_handler", 10, 0
msg1:
    db "hd_interrupt_handler", 10, 0

exception_msg:
    db "exception_handler", 10, 0