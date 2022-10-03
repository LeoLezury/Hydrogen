

#pragma once

#include <stdlib.h>

typedef unsigned char byte;

#define PAYLOAD_TIME 10

#undef assert
extern int assert_fail(const char *file, const char *func, const int line, const char *expr, const char *message) __THROW;
#define assert(expr, ...) ((expr) ? 1 : assert_fail(__FILE__, __func__, __LINE__, #expr, "" __VA_ARGS__))

ulong gettime();
