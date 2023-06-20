#include "../include/linux/kernel.h"
#include "../include/linux/task.h"
#include "../include/linux/mm.h"
#include "../include/string.h"
#include "../include/linux/sched.h"

extern task_t* current;
extern int jiffy;
extern int cpu_tickes;
/**
 * sched.asm中
 */
extern void sched_task();
// 进入用户态
extern void move_to_user_mode();

/*
 * 所有的task
 */
task_t* tasks[NR_TASKS] = {0};

/*
 * 获取一个空闲的process，给进程放一个坑
 * 返回：找到了坑位返回PID，找不到坑位返回-1
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
        printk("no valid pid\n");
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

    task->task.priority = priority;
    task->task.counter = priority;

    task->task.scheduling_times = 0;

    strcpy(task->task.name, name);

    // 加入tasks
    tasks[task->task.pid] = &(task->task);

    task->task.tss.cr3 = virtual_memory_init();
    task->task.tss.eip = fun;                           //功能函数

    // r0 stack
    task->task.esp0 = (int)task + PAGE_SIZE;            //这个程序是运行在内核态的，保存内核态的栈顶
    task->task.ebp0 = task->task.esp0;                  //ring0栈底

    // r3 stack
    task->task.esp3 = malloc(4096) + PAGE_SIZE;
    task->task.ebp3 = task->task.esp3;
    task->task.tss.esp = task->task.esp3;
    task->task.tss.ebp = task->task.ebp3;

    task->task.tss.esp0 = task->task.esp0;

    task->task.state = TASK_READY;

    return task;
}

/*
 * 临时的任务
 */
void* t1_fun(void* arg)
{
    for (int i = 0; i < 0xffffffff; ++i)
    {
        printk("t1: %d\n", i);
        task_sleep(1000);
    }
}
void* t2_fun(void* arg)
{
    for (int i = 0; i < 0xffffffff; ++i)
    {
        printk("t2: %d\n", i);
        task_sleep(500);
    }
}
void* t3_fun(void* arg)
{
    for (int i = 0; i < 0xffffffff; ++i)
    {
        printk("t3: %d\n", i);
        task_sleep(300);
    }
}

/*
 * 实际任务
 */
void* idle(void* arg)
{
//    create_task("init", move_to_user_mode, 1);
    create_task("t1", t1_fun, 2);   //创建一个任务
    create_task("t2", t2_fun, 3);
    create_task("t3", t3_fun, 4);

    while (true) {
        //printk("idle task running...\n");

        __asm__ volatile ("sti;");
        __asm__ volatile ("hlt;");
    }
}

/*
 * 初始化task
 */
void task_init()
{
    create_task("idle", idle, 0);
}

/*
 * 获取父进程id
 */
pid_t get_task_ppid(task_t* task)
{
    return task->ppid;
}

/*
 * 让这个进程的已经执行的时间片增加
 */
int inc_scheduling_times(task_t* task)
{
    return task->scheduling_times++;
}

/*
 * 退出进程
 */
void task_exit(int code, task_t* task)
{
    for (int i = 1; i < NR_TASKS; ++i)
    {
        task_t* tmp = tasks[i];

        if (task == tmp)
        {
            printk("task exit: %s\n", tmp->name);

            tmp->exit_code = code;

            // 先移除，后面有父子进程再相应处理
            tasks[i] = NULL;

            current = NULL;

            free_s(tmp, 4096);      //这里是因为在申请空间的时候就申请了4096，见本文件的create_task函数

            break;
        }
    }
}

/**
 * 推出当前任务
 * @param code
 */
void current_task_exit(int code)
{
    for (int i = 1; i < NR_TASKS; ++i)
    {
        task_t* tmp = tasks[i];

        if (current == tmp) {
            printk("task exit: %s\n", tmp->name);

            tmp->exit_code = code;

            // 先移除，后面有父子进程再相应处理
            tasks[i] = NULL;

            current = NULL;

            break;
        }
    }
}

/**
 * 让task休眠
 * @param ms
 */
void task_sleep(int ms)
{
    CLI

    if (NULL == current)
    {
        printk("task sleep: current task is null\n");
        return;
    }

    int ticks = ms / jiffy;
    ticks += (0 == ms % jiffy)? 0 : 1;

    current->counter = cpu_tickes + ticks;      //ticks为我们想要让这个进程休眠的时间的换算成cpu_ticks
    current->state = TASK_SLEEPING;

    sched_task();
}

/*
 * 唤醒task
 */
void task_wakeup()
{
    for (int i = 1; i < NR_TASKS; ++i)
    {
        task_t* task = tasks[i];

        if (NULL == task) continue;
        if (TASK_SLEEPING != task->state) continue;

        //正在睡眠的task要执行下面的代码
        if (cpu_tickes >= task->counter)
        {
            task->state = TASK_READY;
            task->counter = task->priority;
        }
    }
}

/*
 * 获得esp ring3级别
 */
int get_esp3(task_t* task)
{
    return task->esp3;
}