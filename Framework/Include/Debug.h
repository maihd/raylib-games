#pragma once

#include <stdio.h>
#include <assert.h>

#define DebugAssert(test, message, ...) assert((test) && (message))

#if defined(_MSC_VER)
#define RuntimeAssert(test, message, ...)           \
    do {                                            \
        if (!(test)) {                              \
            printf(message "\n", ##__VA_ARGS__);    \
            __debugbreak();                         \
        }                                           \
    } while (0)
#else
#define RuntimeAssert(test, message, ...)           \
    do {                                            \
        if (!(test)) {                              \
            printf(message "\n", ##__VA_ARGS__);    \
        }                                           \
    } while (0)
#endif
