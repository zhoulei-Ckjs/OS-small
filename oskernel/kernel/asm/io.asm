[bits 32]
[SECTION .text]

;in_byte(int port)
;主调函数需要先将端口push进栈中。
global in_byte              ; 读取一个字节
in_byte:
    push ebp;               ;保存栈底
    mov ebp, esp            ;新栈底

    xor eax, eax

    ;-----------------------------栈分布-----------------------------
    ;   esp+8       参数压栈，端口（主调函数负责压栈参数，从右向左压入栈） |
    ;   esp+4       返回地址（call函数会将返回地址压栈）                |
    ;   esp -->     ebp                                             |
    ;----------------------------------------------------------------

    mov edx, [ebp + 8]      ; 读取端口
    in al, dx               ; 从端口读取一个字节

    leave
    ret

;out_byte(port, value)
;发送一个字节到相应端口
global out_byte
out_byte:
    push ebp;
    mov ebp, esp

    ;-----------------------------栈分布-------------------------------------
    ;   esp+12      参数压栈，需要写入的值（主调函数负责压栈参数，从右向左压入栈）  |
    ;   esp+8       参数压栈，端口（主调函数负责压栈参数，从右向左压入栈）         |
    ;   esp+4       返回地址（call函数会将返回地址压栈）                        |
    ;   esp -->     ebp                                                     |
    ;------------------------------------------------------------------------

    mov edx, [ebp + 8]      ; port
    mov eax, [ebp + 12]     ; value
    out dx, al

    leave
    ret

;in_word(port)
;从端口读取一个字节（16 bit），保存到ax寄存器中
global in_word
in_word:
    push ebp;
    mov ebp, esp

    ;-----------------------------栈分布-----------------------------
    ;   esp+8       参数压栈，端口（主调函数负责压栈参数，从右向左压入栈） |
    ;   esp+4       返回地址（call函数会将返回地址压栈）                |
    ;   esp -->     ebp                                             |
    ;----------------------------------------------------------------

    xor eax, eax

    mov edx, [ebp + 8]      ; port
    in ax, dx

    leave
    ret

;out_byte(port, value)
;发送一个字节到相应端口
global out_word
out_word:
    push ebp;
    mov ebp, esp

    ;-----------------------------栈分布-------------------------------------
    ;   esp+12      参数压栈，需要写入的值（主调函数负责压栈参数，从右向左压入栈）  |
    ;   esp+8       参数压栈，端口（主调函数负责压栈参数，从右向左压入栈）         |
    ;   esp+4       返回地址（call函数会将返回地址压栈）                        |
    ;   esp -->     ebp                                                     |
    ;------------------------------------------------------------------------

    mov edx, [ebp + 8]      ; port
    mov eax, [ebp + 12]     ; value
    out dx, ax

    leave
    ret