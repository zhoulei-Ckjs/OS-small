#ifndef OS_ASSERT_H
#define OS_ASSERT_H

/**
 * 函数被调用当内核遇到一个无法恢复或者处理的致命错误时。当发生这种类型的错误时，系统的持续运行可能会导致更多的错误，或者产生不可预见的结果。
 * 因此，内核选择停止运行，防止可能的进一步损害。
 * @param fmt
 * @param ...
 */
void panic(const char *fmt, ...);

#endif //OS_ASSERT_H
