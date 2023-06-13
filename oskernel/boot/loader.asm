[ORG  0x500]

[SECTION .data]
KERNEL_ADDR equ 0x1200          ;内核的入口地址

[SECTION .gdt]
SEG_BASE equ 0
SEG_LIMIT equ 0xfffff           ;20位寻址能力

;显示器的地址和大小
B8000_SEG_BASE equ 0xb8000
B8000_SEG_LIMIT equ 0x7fff

;段选择子
CODE_SELECTOR equ (1 << 3)      ;这里一个gdt描述符占8位，所以这是第一个即gdt_code的选择子
DATA_SELECTOR equ (2 << 3)      ;同上，这是gdt_data的选择子、
B8000_SELECTOR equ (3 << 3)     ;显示器的选择子

gdt_base:
    dd 0, 0                     ;最开始的GDT表
gdt_code:
    dw SEG_LIMIT & 0xffff       ;段界限（15-0）
    dw SEG_BASE & 0xffff        ;段基址（31-16）
    db SEG_BASE >> 16 & 0xff    ;段基址（39-32）
    ;    P_DPL_S_TYPE
    db 0b1_00_1_1000            ;段描述符有效_工作在ring0_非系统段_仅具有执行权限
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | (SEG_LIMIT >> 16 & 0xf)      ;以4K为单位_32位段_非64位代码段_段界限（最高4位）
    db SEG_BASE >> 24 & 0xff    ;段基址
gdt_data:
    dw SEG_LIMIT & 0xffff
    dw SEG_BASE & 0xffff
    db SEG_BASE >> 16 & 0xff
    ;    P_DPL_S_TYPE
    db 0b1_00_1_0010            ;段描述符有效_工作在ring0_非系统段_仅具有只读权限
    ;    G_DB_AVL_LIMIT
    db 0b1_1_00_0000 | (SEG_LIMIT >> 16 & 0xf)      ;以4KB为单位_32位段_非64位代码段_段界限（最高4位）
    db SEG_BASE >> 24 & 0xff
gdt_b8000:                                          ;显示器的gdt表
    dw B8000_SEG_LIMIT & 0xffff
    dw B8000_SEG_BASE & 0xffff
    db B8000_SEG_BASE >> 16 & 0xff
    ;    P_DPL_S_TYPE
    db 0b1_00_1_0010            ;段描述符有效_工作在ring0_非系统段_仅具有只读权限
    ;    G_DB_AVL_LIMIT
    db 0b0_1_00_0000 | (B8000_SEG_LIMIT >> 16 & 0xf) ;以字节为单位_32位段_非64位代码段_段界限（最高4位）
    db B8000_SEG_BASE >> 24 & 0xff
gdt_ptr:
    dw $ - gdt_base - 1         ;这里应该是gdtlen - 1
    dd gdt_base                 ;GDT基地址

[SECTION .text]
[BITS 16]
global loader_start
loader_start:
    mov     ax, 0
    mov     ss, ax
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     si, ax

    mov     si, prepare_enter_protected_mode_msg
    call    print

enter_protected_mode:
    ; 关中断
    cli
;xchg bx,bx  ;---------------------------------------------------------------------------------
    ; 加载gdt表
    lgdt  [gdt_ptr]


    ;测试代码
    ;mov byte [0x10000], 0x0b    ;不开启A20地址总线的话，会地址环绕，将0x10000变成0x10000-0xffff=0x1，就不能使用0-4G空间
    ;xchg bx,bx

    ; 开A20
    in    al,  92h
    or    al,  00000010b
    out   92h, al

    ; 设置保护模式
    mov   eax, cr0
    or    eax , 1           ;最低位置1开启保护模式
    mov   cr0, eax
;xchg bx,bx  ;---------------------------------------------------------------------------------
    jmp CODE_SELECTOR:protected_mode

; 如何调用
; mov     si, msg   ; 1 传入字符串
; call    print     ; 2 调用
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

[BITS 32]
protected_mode:
    mov ax, DATA_SELECTOR
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x9fbff
;xchg bx,bx  ;---------------------------------------------------------------------------------
    ; 将内核读入内存
    mov edi, KERNEL_ADDR        ;要读取到的地址
    mov ecx, 3                  ;从那个扇区开始读
    mov bl, 60                  ;读多少个扇区
    call read_hd                ;读盘
;xchg bx,bx  ;---------------------------------------------------------------------------------
    jmp CODE_SELECTOR:KERNEL_ADDR

read_hd:
    ; 0x1f2 8bit 指定读取或写入的扇区数
    mov dx, 0x1f2
    mov al, bl
    out dx, al

    ; 0x1f3 8bit iba地址的第八位 0-7
    inc dx
    mov al, cl
    out dx, al

    ; 0x1f4 8bit iba地址的中八位 8-15
    inc dx
    mov al, ch
    out dx, al

    ; 0x1f5 8bit iba地址的高八位 16-23
    inc dx
    shr ecx, 16
    mov al, cl
    out dx, al

    ; 0x1f6 8bit
    ; 0-3 位iba地址的24-27
    ; 4 0表示主盘 1表示从盘
    ; 5、7位固定为1
    ; 6 0表示CHS模式，1表示LAB模式
    inc dx
    shr ecx, 8
    and cl, 0b1111
    mov al, 0b1110_0000     ; LBA模式
    or al, cl
    out dx, al

    ; 0x1f7 8bit  命令或状态端口
    inc dx
    mov al, 0x20
    out dx, al

    ; 设置loop次数，读多少个扇区要loop多少次
    mov cl, bl
.start_read:
    push cx     ; 保存loop次数，防止被下面的代码修改破坏

    call .wait_hd_prepare
    call read_hd_data

    pop cx      ; 恢复loop次数

    loop .start_read

.return:
    ret

; 一直等待，直到硬盘的状态是：不繁忙，数据已准备好
; 即第7位为0，第3位为1，第0位为0
.wait_hd_prepare:
    mov dx, 0x1f7

.check:
    in al, dx
    and al, 0b1000_1000
    cmp al, 0b0000_1000
    jnz .check

    ret

; 读硬盘，一次读两个字节，读256次，刚好读一个扇区
read_hd_data:
    mov dx, 0x1f0
    mov cx, 256

.read_word:
    in ax, dx
    mov [edi], ax
    add edi, 2
    loop .read_word

    ret

prepare_enter_protected_mode_msg:
    db "Prepare to go into protected mode...", 10, 13, 0