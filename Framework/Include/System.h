#pragma once

void SystemError(const char* format, ...);
void SystemErrorDebug(const char* func, const char* file, int line, const char* format, ...);

#if defined(_MSC_VER)
#define SystemAssert(test, message, ...)            \
    do {                                            \
        if (!(test)) {                              \
            printf(message "\n", ##__VA_ARGS__);    \
            __debugbreak();                         \
        }                                           \
    } while (0)
#else
#define SystemAssert(test, message, ...)            \
    do {                                            \
        if (!(test)) {                              \
            printf(message "\n", ##__VA_ARGS__);    \
        }                                           \
    } while (0)
#endif
