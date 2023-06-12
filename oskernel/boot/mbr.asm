;0柱面0磁道1扇区
[ORG  0x7c00]

[SECTION .data]
LOADER_MAIN_ADDR equ 0x500    ;将loader读入到这个地址

[SECTION .text]
[BITS 16]
global _start
_start:

    xchg bx, bx ;-------------------------------------------------------------------------------------------

;设置屏幕模式为文本模式，清除屏幕
    mov ax, 3
    int 0x10

    ;读盘
    mov ch, 0               ;0 柱面
    mov dh, 0               ;0 磁头
    mov cl, 2               ;第二个扇区
    mov bx, LOADER_MAIN_ADDR;读取到这个地址

    mov ah, 0x02            ;读盘操作
    mov al, 1               ;连续读几个扇区
    mov dl, 0               ;驱动器编号

    xchg bx, bx ;-------------------------------------------------------------------------------------------

    int 0x13

    ;跳过去
    mov si, jmp_to_loader
    call print

    xchg bx, bx ;-------------------------------------------------------------------------------------------

    jmp LOADER_MAIN_ADDR

read_floppy_error:
    mov si, read_floppy_error_msg
    call print

    jmp $

print:
    mov ah, 0x0e
    mov bh, 0
    mov bl, 0x01
.loop:
    mov al, [si]
    cmp al, 0
    jz .done
    int 0x10

    inc si
    jmp .loop
.done:
    ret

read_floppy_error_msg:
    db "read floppy error!", 10, 13, 0

jmp_to_loader:
    db "jump to loader...", 10, 13, 0

times 510 - ($ - $$) db 0
db 0x55, 0xaa