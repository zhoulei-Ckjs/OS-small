/*
 * 定义显示模式CGA的字体颜色
 * */

#ifndef OS_SYSTEM_H
#define OS_SYSTEM_H

#include "../linux/types.h"

#define DEBUG

#define BOCHS_DEBUG_MAGIC   __asm__("xchg bx, bx");     //bochs断点

#define STI   __asm__("sti");       //开中断
#define CLI   __asm__("cli");       //关中断

/*
    定义字体颜色，往屏幕上写的时候
    ah存储颜色，al存储字符，如写到显存开始处0xb8000
*/
typedef enum {
    rc_black = 0,
    rc_blue = 1,
    rc_green = 2,
    rc_cyan = 3,
    rc_red = 4,
    rc_magenta = 5,
    rc_brown = 6,
    rc_light_grey = 7,
    rc_dark_grey = 8,
    rc_light_blue = 9,
    rc_light_green = 10,
    rc_light_cyan = 11,
    rc_light_red = 12,
    rc_light_magenta = 13,
    rc_light_brown = 14,
    rc_white = 15
} real_color_t;

#endif //OS_SYSTEM_H
