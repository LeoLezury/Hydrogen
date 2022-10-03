

#include <stdio.h>
#include <time.h>

#include "basic.h"

int assert_fail(const char *file, const char *func, const int line, const char *expr, const char *message)
{
    if (message)
        printf("\e[38;2;255;0;0m[%s] [%s:%d] 断言 %s 失败 %s\e[0m\n", file, func, line, expr, message);
    else
        printf("\e[38;2;255;0;0m[%s] [%s:%d] 断言 %s 失败\e[0m\n", file, func, line, expr);
    return 0;
}

ulong gettime()
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec * (ulong)1e9 + t.tv_nsec;
}
