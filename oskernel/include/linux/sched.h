#ifndef OS_SCHED_H
#define OS_SCHED_H

/*
 * 进行任务调度
 */
void sched();

/*
 * 一个时间片到了，要做一些处理
 */
void do_timer();

#endif //OS_SCHED_H
