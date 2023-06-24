[bits 32]
[SECTION .text]

;用于保存当前硬盘下一次发生中断该如何处理的函数，如读命令，发生中断后这里应该保存读函数
;hd_lba28.c中定义
extern dev_interrupt_handler

;硬盘中断入口
global hd_handler_entry
hd_handler_entry:
    call [dev_interrupt_handler]

    iret