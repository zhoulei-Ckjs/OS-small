#include "../include/linux/task.h"
#include "../include/linux/mm.h"
#include "../include/string.h"

extern task_t* current;

/*
 * 所有的task
 */
task_t* tasks[NR_TASKS] = {0};

/*
 * 获取一个空闲的process
 * 返回：PID
 */
int find_empty_process()
{
    int ret = 0;

    bool is_finded = false;

    for (int i = 0; i < NR_TASKS; ++i)
    {
        if (0 == tasks[i])
        {
            is_finded = true;

            ret = i;
            break;
        }
    }

    if (!is_finded)
    {
        printf("no valid pid\n");
        return -1;
    }

    return ret;
}

/*
 * 创建任务
 */
task_t* create_task(char* name, task_fun_t fun, int priority)
{
    task_union_t* task = (task_union_t*)malloc(4096);
    memset(task, 0, PAGE_SIZE);

    task->task.pid = find_empty_process();
    task->task.ppid = (NULL == current)? 0 : current->pid;  //父进程PID

    task->task.scheduling_times = 0;

    strcpy(task->task.name, name);

    // 加入tasks
    tasks[task->task.pid] = &(task->task);

    task->task.tss.cr3 = (int)task + sizeof(task_t);    //获得了联合体task_union_t的栈起始地址（为什么？）
    task->task.tss.eip = fun;

    // r0 stack
    task->task.esp0 = (int)task + PAGE_SIZE;            //这个程序是运行在内核态的，保存内核态的栈顶
    task->task.ebp0 = task->task.esp0;                  //ring0栈底

    task->task.tss.esp0 = task->task.esp0;

    task->task.state = TASK_READY;

    return task;
}

/*
 * 实际任务
 */
void* idle(void* arg)
{
    printf("#1 idle task running...\n");

    while (true)
    {
        printf("#2 idle task running...\n");

        __asm__ volatile ("sti;");      //开中断
        __asm__ volatile ("hlt;");      //CPU 停止执行指令，直到有中断到来
    }
}

/*
 * 初始化task
 */
void task_init()
{
    create_task("idle", idle, 1);
}