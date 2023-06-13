#include "../include/linux/tty.h"
#include "../include/linux/kernel.h"

void kernel_main(void)
{
    console_init();

    char* s = "myos";

    for (int i = 0; i < 20; ++i) {
        printf("name: %s, age:%d\n", s, i);
    }

    while (true);
}
