#ifndef OS_TASK_H
#define OS_TASK_H

#include "mm.h"

// 进程上限
#define NR_TASKS 64

typedef void* (*task_fun_t)(void*);     //定义一个函数指针

typedef struct task_t {                 //任务的文件描述符
    int             pid;
    int             cr3;
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
 */
task_t* create_task(char* name, task_fun_t fun);

void task_init();                       //初始化task

#endif //OS_TASK_H
