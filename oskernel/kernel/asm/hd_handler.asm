[bits 32]
[SECTION .text]

;用于保存当前硬盘下一次发生中断该如何处理的函数，如读命令，发生中断后这里应该保存读函数
;hd_lba28.c中定义
extern dev_interrupt_handler
; 当前进程
; 定义在sched.c中
extern current
; 打印字符串，内核函数
; 在printk.c中定义
extern printk

;硬盘中断入口
global hd_handler_entry
hd_handler_entry:

    push ecx
    mov ecx, [current]  ; 如果current为空就报错
    cmp ecx, 0
    je .hd_handler_fail

; 保存现场环境
.save_env:
    mov [ecx + 10 * 4], eax
    mov [ecx + 12 * 4], edx
    mov [ecx + 13 * 4], ebx
    mov [ecx + 15 * 4], ebp
    mov [ecx + 16 * 4], esi
    mov [ecx + 17 * 4], edi

    mov eax, [esp + 4]          ; eip 由于栈顶地址在进行函数调用时通常用来保存返回地址，
                                ; 而返回地址就是下一条要执行的指令的地址，所以在函数中可以通过ESP寄存器加上一个偏移量来访问返回地址
    mov [ecx + 8 * 4], eax      ; tss.eip

    mov eax, esp
    add eax, 0x10               ; 这里是找到了最初的esp
    mov [ecx + 4], eax          ; tss.esp0，ring0栈顶地址

    mov eax, ecx
    pop ecx
    mov [eax + 11 * 4], ecx

.call_handler:
    call [dev_interrupt_handler]    ;调用真正的硬盘中断处理函数
    iret

; 当前进程为空，有问题，task没有创建就发生硬盘中断
.hd_handler_fail:
    pop ecx

    push fail_msg
    call printk
    add esp, 4

    iret

fail_msg:
    db "hd handler fail", 10, 0