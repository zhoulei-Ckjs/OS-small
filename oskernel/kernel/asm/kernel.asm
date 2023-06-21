[BITS 32]

extern user_mode
extern _exit
extern get_esp3

extern current

global move_to_user_mode

[SECTION .data]
R3_CODE_SELECTOR equ (4 << 3 | 0b11)
R3_DATA_SELECTOR equ (5 << 3 | 0b11)

[SECTION .text]

; eip
; cs
; eflags
; esp3
; ss3
move_to_user_mode:
    mov esi, [current]

    push esi
    call get_esp3
    add esp, 4                  ; 平栈，将esi推出栈

    push R3_DATA_SELECTOR       ; ss，当CPU执行入栈（push）或出栈（pop）指令时，会根据ss指向的段地址和esp指向的偏移地址来访问堆栈中的数据。
    push eax                    ; esp
    pushf                       ; eflags

    mov ax, R3_CODE_SELECTOR
    push eax                    ; cs

    push user_mode_handler      ; eip

    mov ax, R3_DATA_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    iretd                       ; 此指令通过以相反顺序弹出堆栈上的值（EIP，CS，EFLAGS，ESP和SS），然后恢复执行用户模式代码来执行返回到用户模式。

user_mode_handler:
    call user_mode

    push 0
    call _exit
    add esp, 4

    ; 下面这两句正常情况执行不到,一种保险策略
    sti
    hlt

