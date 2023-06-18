#include "../include/linux/sched.h"
#include "../include/linux/task.h"

/*
 * sched.asm中
 */
extern void switch_idle_task(task_t* task);

extern task_t* tasks[NR_TASKS];

task_t* current = NULL;     //当前任务

/*
 * 找下一个就绪的task
 */
task_t* find_ready_task()
{
    task_t* next = NULL;

    for (int i = 1; i < NR_TASKS; ++i)      //从第一个开始搜索，而不是第0个
    {
        task_t* task = tasks[i];

        if (NULL == task)
            continue;
        if (TASK_READY != task->state)
            continue;

        next = task;
    }

    return next;
}

void sched()
{
    task_t* next = find_ready_task();

    if (NULL == next) {
        current = tasks[0];

        switch_idle_task(current);

        return;
    }
}
