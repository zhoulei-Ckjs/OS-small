#ifndef OS_TASK_H
#define OS_TASK_H

#include "mm.h"

// 进程上限
#define NR_TASKS 64

typedef void* (*task_fun_t)(void*);     //定义一个函数指针

/*
 * 任务状态
 */
typedef enum task_state_t
{
    TASK_INIT,     // 初始化
    TASK_RUNNING,  // 执行
    TASK_READY,    // 就绪
    TASK_BLOCKED,  // 阻塞
    TASK_SLEEPING, // 睡眠
    TASK_WAITING,  // 等待
    TASK_DIED,     // 死亡
} task_state_t;

/*
 * task stage segment
 * 进行任务切换时保存任务状态
 */
typedef struct tss_t
{
    u32 backlink; // 前一个任务的链接，保存了前一个任状态段的段选择子
    u32 esp0;     // ring0 的栈顶地址
    u32 ss0;      // ring0 的栈段选择子
    u32 esp1;     // ring1 的栈顶地址
    u32 ss1;      // ring1 的栈段选择子
    u32 esp2;     // ring2 的栈顶地址
    u32 ss2;      // ring2 的栈段选择子
    u32 cr3;
    u32 eip;        //指向程序下一条指令的地址
    u32 flags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldtr;          // 局部描述符选择子
    u16 trace : 1;     // 如果置位，任务切换时将引发一个调试异常
    u16 reversed : 15; // 保留不用
    u16 iobase;        // I/O 位图基地址，16 位从 TSS 到 IO 权限位图的偏移
    u32 ssp;           // 任务影子栈指针
} __attribute__((packed)) tss_t;

/*
 * task的结构体
 */
typedef struct task_t
{
    tss_t           tss;
    int             pid;
    int             ppid;
    char            name[32];
    task_state_t    state;
    int             exit_code;
    int             counter;
    int             priority;
    int             scheduling_times;       // 调度次数
    int             esp0;                   // 刚开始创建的时候 活动的esp3保存在tss中
    int             ebp0;
    int             esp3;
    int             ebp3;
    int             magic;
}task_t;

/*
 * task联合体，4096字节，与页表大小一致
 */
typedef union task_union_t {
    task_t task;
    char stack[PAGE_SIZE];
}task_union_t;

/*
 * 创建任务
 * 参数1：task名字
 * 参数2：功能函数
 * 参数3：优先级
 */
task_t* create_task(char* name, task_fun_t fun, int priority);

void task_init();                       //初始化task

#endif //OS_TASK_H
