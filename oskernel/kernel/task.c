#include "../include/linux/task.h"
#include "../include/linux/mm.h"

/*
 * 所有的task
 */
task_t* tasks[NR_TASKS] = {0};

/*
 * 获取一个空闲的process
 * 返回：PID
 */
static int find_empty_process() {
    int ret = 0;

    bool is_finded = false;

    for (int i = 0; i < NR_TASKS; ++i) {
        if (0 == tasks[i]) {
            is_finded = true;

            ret = i;
            break;
        }
    }

    if (!is_finded) {
        printf("no valid pid\n");
        return -1;
    }

    return ret;
}

/*
 * 创建任务
 */
task_t* create_task(char* name, task_fun_t fun) {
    task_union_t* task = (task_union_t*)get_free_page();
    memset(task, 0, PAGE_SIZE);

    task->task.pid = find_empty_process();
    tasks[task->task.pid] = &(task->task);      //将 PCB 填入 task表

    task->task.cr3 = virtual_memory_init();     //获得当前进程的页表 首地址

    return task;
}

/*
 * 实际任务
 */
void* idle(void* arg) {

}

/*
 * 初始化task
 */
void task_init() {
    create_task("idle", idle);
}