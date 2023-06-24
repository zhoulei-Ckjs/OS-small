#include "../include/linux/hd.h"

void kernel_thread_fun(void* arg)
{
    hd_init();
}