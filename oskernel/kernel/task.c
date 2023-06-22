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
/**
 * 进入用户态
 * 在kernel.asm中
 */
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
    task_union_t* task = (task_union_t*)kmalloc(4096);
    memset(task, 0, PAGE_SIZE);

    task->task.pid = find_empty_process();
    task->task.ppid = (NULL == current)? 0 : current->pid;  //父进程PID

    task->task.priority = priority;
    task->task.counter = priority;

    task->task.scheduling_times = 0;

    strcpy(task->task.name, name);

    // 加入tasks
    tasks[task->task.pid] = &(task->task);

//    task->task.tss.cr3 = virtual_memory_init();
    task->task.tss.cr3 = (int)task + sizeof(task_t);
    task->task.tss.eip = fun;                           //功能函数

    // r0 stack
    task->task.esp0 = (int)task + PAGE_SIZE;            //这个程序是运行在内核态的，保存内核态的栈顶
    task->task.ebp0 = task->task.esp0;                  //ring0栈底

    // r3 stack
    task->task.esp3 = kmalloc(4096) + PAGE_SIZE;
    task->task.ebp3 = task->task.esp3;
    task->task.tss.esp = task->task.esp3;
    task->task.tss.ebp = task->task.ebp3;

    task->task.tss.esp0 = task->task.esp0;

    task->task.state = TASK_READY;

    return task;
}

/**
 * 创建紫禁城
 * @param name
 * @param fun
 * @param priority
 * @return
 */
task_t* create_child(char* name, task_fun_t fun, int priority)
{
    if (NULL == current)
    {
        printk("[%s:%d] current task is null\n", __FILE__, __LINE__);
        return NULL;
    }

    task_union_t* task = (task_union_t*)kmalloc(4096);
    memset(task, 0, PAGE_SIZE);

    // 拷贝父进程的内存空间
    memcpy(task, current, PAGE_SIZE);

    // 设置个性化数据
    task->task.pid = find_empty_process();

    task->task.ppid = (NULL == current)? 0 : current->pid;

    task->task.priority = priority;
    task->task.counter = priority;
    task->task.scheduling_times = 0;

    strcpy(task->task.name, name);

    // 加入tasks
    tasks[task->task.pid] = &(task->task);

    task->task.tss.cr3 = (int)task + sizeof(task_t);        //这里好像有问题，cr3寄存器为啥直接等于这个
    task->task.tss.eip = fun;

    // r0 stack
    task->task.esp0 = (int)task + PAGE_SIZE;
    task->task.ebp0 = task->task.esp0;

    task->task.tss.esp0 = task->task.esp0;

    // r3 stack
    task->task.esp3 = kmalloc(4096) + PAGE_SIZE;
    task->task.ebp3 = task->task.esp3;

    task->task.tss.esp = task->task.esp3;
    task->task.tss.ebp = task->task.ebp3;

    // 将父进程r3 stack拷贝到子进程r3 stack
    memcpy(task->task.esp3 - PAGE_SIZE, current->esp3 - PAGE_SIZE, PAGE_SIZE);

    // 计算子进程的esp
    int parent_esp_used = current->esp3 - current->tss.esp;
    int parent_ebp_used = current->ebp3 - current->tss.ebp;

    // 不会覆盖父进程的堆栈信息，在此之上重新建立栈顶栈底
    task->task.tss.esp -= parent_esp_used;
    task->task.tss.ebp -= parent_ebp_used;

    task->task.state = TASK_READY;

    return task;
}

/*
 * 实际任务
 */
void* idle(void* arg)
{
    create_task("init", move_to_user_mode, 1);


    while (true)
    {
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
    create_task("idle", idle, 0);       //idle是功能函数，创建0号任务
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

pid_t get_task_pid(task_t* task)
{
    return task->pid;
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

            free_s(tmp->esp3, 4096);
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

            free_s(tmp->esp3 - PAGE_SIZE, 4096);
            free_s(tmp, 4096);

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
void set_esp3(task_t* task, int esp) {
    task->tss.esp = esp;
}