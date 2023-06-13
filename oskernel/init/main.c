#include "../include/linux/tty.h"
//#include "../include/linux/kernel.h"

void kernel_main(void)
{
    console_init();

    char* s = "myos";

    console_write(s, 5);

    while (true);
}
