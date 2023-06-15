[SECTION .text]
[BITS 32]
extern kernel_main

global _start

_start:
;xchg bx,bx  ;----------------------------------------------------------

; 配置8259a芯片，响应中断用
.config_8a59a:
    ; 向主发送ICW1
    mov al, 11h
    out 20h, al         ;发送  0001 0001      必须设置为1_需要发送ICW4

    ; 向从发送ICW1
    out 0a0h, al        ;发送  0001 0001      必须设置为1_需要发送ICW4

    ; 向主发送ICW2
    mov al, 20h
    out 21h, al         ;0010 0000           ???_中断向量表的起始地址为0x00（从00开始，0-31是处理系统默认中断）

    ; 向从发送ICW2
    mov al, 28h
    out 0a1h, al        ;0010 1000          ???_中断向量表的起始地址为0x20（32，可能是从芯片处理自定义中断）

    ; 向主发送ICW3
    mov al, 04h
    out 21h, al         ;0000 0100          IR2级联从片(负责与级联芯片通信)

    ; 向从发送ICW3
    mov al, 02h
    out 0A1h , al       ;0000 0010          IR1级联从片（与主片通信）

    ; 向主发送ICW4
    mov al, 003h
    out 021h, al        ;0000 0011          自动EOI(end of interrupt)_x86模式

    ; 向从发送ICW4
    out 0A1h, al        ;0000 0011          自动EOI(end of interrupt)_x86模式

    ; 屏蔽所有中断，只接收键盘中断
.enable_8259a_main:
    ;mov al, 11111101b           ;只接收键盘中断
    ;mov al, 11111100b           ;接收键盘中断和时钟中断
    mov al, 11111111b           ;关闭中断
    out 21h, al

    ; 屏蔽从芯片所有中断响应
.disable_8259a_slave:
    mov al, 11111111b
    out 0A1h, al

    ; 调用kernel程序
.enter_c_word:
    call kernel_main

    jmp $