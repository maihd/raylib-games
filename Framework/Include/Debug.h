#pragma once

#include <stdio.h>
#include <assert.h>

#define DebugAssert(test, message, ...) assert((test) && (message))

#define DebugPrint(format, ...) DebugPrintWithSource(__FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__)

void DebugPrintWithSource(const char* func, const char* file, int line, const char* message, ...);
