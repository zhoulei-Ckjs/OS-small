void kernel_main(void)
{
    int a = 0;

    *(char *)0x100000 = 0x11;

    char* video = (char*)0xb8000;
    *video = 'G';
}