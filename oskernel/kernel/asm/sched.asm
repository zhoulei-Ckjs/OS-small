[BITS 32]
[SECTION .text]

; 在sched.c中定义的，用于指向当前运行的任务
extern current

; 获取父进程id
; 在task.c中
; pid_t get_task_ppid(task_t* task)
extern get_task_ppid
; 让一个进程已经运行的时间片增加
; 在task.c中
; int inc_scheduling_times(task_t* task)
extern inc_scheduling_times
; 退出进程
; 在task.c中
; void task_exit(int code, task_t* task)
extern task_exit
; 进行cpu任务调度
; 在sched.c中
; void sched()
extern sched

; void switch_task(task_t* task)
global switch_task
switch_task:
    ; 恢复上下文
    mov eax, [current]

    ; 恢复ebp0 esp0，内核的栈顶和栈底
    mov esp, [eax + 4]
    mov ebp, [eax + 15 * 4]

    push eax                        ; 获取父进程ID，如果不为0表示子进程，不需要压入task_exit_handler
    call get_task_ppid              ; 主调函数负责平栈
    add esp, 4                      ; 调用后平栈
    cmp eax, 0
    jne .recover_env                ; 父进程不为0

    ; 父进程为0
    mov eax, [current]
    push eax                        ;进程号
    call inc_scheduling_times       ; 返回当前任务的已经调度了多少时间，并将其 +1
    add esp, 4                      ; 调用后平栈

    cmp eax, 0
    jne .recover_env                ; 不是第一次调度

    ; 如果是第一次調度
    mov eax, task_exit_handler
    push eax

.recover_env:
    mov eax, [current]

    ; 恢复通用寄存器
    mov ecx, [eax + 11 * 4]
    mov edx, [eax + 12 * 4]
    mov ebx, [eax + 13 * 4]
    mov esi, [eax + 16 * 4]
    mov edi, [eax + 17 * 4]

    mov eax, [eax + 8 * 4]      ; eip什么时候被赋值的？要看task.c::create_task

    sti

    jmp eax                     ; 跳到功能函数执行

;
task_exit_handler:
    mov eax, [current]
    push eax
    push 0                      ; exit code = 0 表示正常退出
    call task_exit
    add esp, 8

    call sched

    ; 下面这两句正常情况执行不到,一种保险策略
    sti
    hlt

; 栈:
;   sched_task return address
;   ...
global sched_task
sched_task:
    xchg bx, bx
    xchg bx, bx

    push ecx

    mov ecx, [current]
    cmp ecx, 0
    je .return

    mov [ecx + 10 * 4], eax
    mov [ecx + 12 * 4], edx
    mov [ecx + 13 * 4], ebx
    mov [ecx + 15 * 4], ebp
    mov [ecx + 16 * 4], esi
    mov [ecx + 17 * 4], edi

    mov eax, [esp + 4]          ; eip
    mov [ecx + 8 * 4], eax      ; tss.eip

    mov eax, esp
    add eax, 8
    mov [ecx + 4], eax          ; tss.esp0

    pop ecx
    mov [ecx + 11 * 4], ecx

.return:
    call sched

    ret