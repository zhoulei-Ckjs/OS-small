#include "../include/linux/sched.h"
#include "../include/linux/task.h"

extern task_t* tasks[NR_TASKS];

task_t* current = NULL;     //当前任务

void sched() {
    current = tasks[0];
}
