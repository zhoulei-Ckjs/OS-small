#include "../include/linux/sched.h"
#include "../include/linux/task.h"
#include "../include/linux/kernel.h"
#include "../include/asm/system.h"

/*
 * sched.asm中
 */
extern void switch_idle_task(task_t* task);

/*
 * 进行下一个任务的切换
 * 在sched.asm中
 */
extern void switch_task(task_t* task);

extern task_t* tasks[NR_TASKS];

task_t* current = NULL;     //当前任务

/*
 * 找下一个就绪的task
 */
task_t* find_ready_task()
{
    task_t* next = NULL;

    // 判断是不是所有任务都被调度了一轮
    bool is_all_zero = true;
    bool is_null = true;    // 任务队列是否为空的标记，除了0号idle

    for (int i = 1; i < NR_TASKS; ++i)
    {
        task_t* task = tasks[i];

        if (NULL == task) continue;

        is_null = false;    // 任务队列不为空

        if (0 != task->counter)
        {
            is_all_zero = false;
            break;
        }
    }

    // 如果没有任务需要调度，is_all_zero也为true，排除这种情况
    if (!is_null && is_all_zero)  goto reset_task;      //任务队列不为空，且全都调度完了一轮

    // 走到这里是任务队列为空 或 所有任务并没有全部调度一轮，所以要找一个还有时间片的返回
    for (int i = 1; i < NR_TASKS; ++i)
    {
        task_t* task = tasks[i];

        if (NULL == task)
        {
            continue;
        }

        if (current == task && TASK_RUNNING == task->state)
        {
            task->state = TASK_READY;
        }

        if (TASK_READY != task->state) continue;

        if (NULL == next)
        {
            next = task;
        }
        else
        {
            if (task->counter > next->counter)
            {
                next = task;
            }
        }
    }

    if (NULL == next)
    {
        next = tasks[0];
    }

    return next;

    /**
     * 如果所有任务的counter都为0,代表所有任务都被调度了一轮
     * 重新赋值
     */
reset_task:
    if (is_all_zero)
    {
        for (int i = 1; i < NR_TASKS; ++i)
        {
            task_t *tmp = tasks[i];

            if (NULL == tmp) continue;

            tmp->counter = tmp->priority;
        }

        // 重新设置counter后，再次查找可调度的任务
        return find_ready_task();
    }
}

/*
 * 获得下一个任务进行执行
 */
void sched()
{
    if (NULL != current)
    {
        if (TASK_SLEEPING != current->state)
        {
            current->state = TASK_READY;
        }

        current = NULL;
    }

    task_t* next = find_ready_task();

    next->state = TASK_RUNNING;         //下一个进程的是运行状态
    current = next;
    switch_task(next);
}

/*
 * 一个时间片到了，要做一些处理
 */
void do_timer()
{
    if (NULL == current)
    {
        sched();                // 获得下一个任务进行执行
    }

    if((NULL != current) && (current->counter > 0))
    {
        current->counter--;     // 将时间片 -1
        return;
    }

    sched();                    // 获得下一个任务进行调度
}
