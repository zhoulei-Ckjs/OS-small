#include "../include/shell.h"
#include "../include/string.h"

bool g_active_shell = false;

// 用于存储键盘输入的shell命令
ushort g_shell_command_off = 0;
char g_shell_command[64] = {0};

void active_shell()
{
    g_active_shell = true;
    printk("shell is activated!\n");
    printk("please input command:");
}

void exec_shell()
{
    if (!g_active_shell) return;

    printk("start exec: %s\n", g_shell_command);

    if (!strcmp("1", g_shell_command))
    {
        printk("run 1\n");
    }

    g_shell_command_off = 0;            //将存储的命令清零
    memset(g_shell_command, 0, 64);

    printk("please input command:");
}

void run_shell(char ch)
{
    if (!g_active_shell) return;

    if (g_shell_command_off >= 64)
    {
        printk("The command length exceeds 64");
    }

    g_shell_command[g_shell_command_off++] = ch;
}

/**
 * 如果按了删除键，前面输入的内容要从g_shell_command中删掉
 */
void del_shell()
{
    if (!g_active_shell) return;

    g_shell_command[--g_shell_command_off] = 0;
}