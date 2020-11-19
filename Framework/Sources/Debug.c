#include <Debug.h>

#include <stdio.h>
#include <stdarg.h>

void DebugPrintWithSource(const char* func, const char* file, int line, const char* message, ...)
{
    int ret;

    ret = fprintf(stderr, "[%s:%d:%s] ", file, line, func);

    va_list vargs;
    va_start(vargs, message);
    ret = vfprintf(stderr, message, vargs);
    va_end(vargs);

    fputc('\n', stderr);
    return ret;
}
