#pragma once

#include <stdint.h>
#include <string.h>

#if !defined(NDEBUG)
#define MemoryAlloc(size)              MemoryAllocDebug(size, __FUNCTION__, __FILE__, __LINE__)
#define MemoryRealloc(ptr, size)       MemoryReallocDebug(ptr, size, __FUNCTION__, __FILE__, __LINE__)
#define MemoryFree(ptr)                MemoryFreeDebug(ptr, __FUNCTION__, __FILE__, __LINE__)

void* MemoryAllocDebug(size_t size, const char* func, const char* file, int line);
void* MemoryReallocDebug(void* ptr, size_t size, const char* func, const char* file, int line);
void  MemoryFreeDebug(void* ptr, const char* func, const char* file, int line);
#else
void* MemoryAlloc(size_t size);
void* MemoryRealloc(void* ptr, size_t size);
void  MemoryFree(void* ptr);
#endif

#define MemoryInit(dst, value, size)   memset(dst, value, size)
#define MemoryCopy(dst, src, size)     memcpy(dst, src, size)
#define MemoryMove(dst, src, size)     memmove(dst, src, size)

void MemoryDumpAllocs(void);
