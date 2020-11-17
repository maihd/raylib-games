#pragma once

#include <stdint.h>
#include <string.h>

#if !defined(NDEBUG)
#define MemoryAlloc(size)              _MemoryAlloc(size, __FUNCTION__, __FILE__, __LINE__)
#define MemoryRealloc(ptr, size)       _MemoryRealloc(ptr, size, __FUNCTION__, __FILE__, __LINE__)
#define MemoryFree(ptr)                _MemoryFree(ptr, __FUNCTION__, __FILE__, __LINE__)

void* _MemoryAlloc(size_t size, const char* func, const char* file, int line);
void* _MemoryRealloc(void* ptr, size_t size, const char* func, const char* file, int line);
void  _MemoryFree(void* ptr, const char* func, const char* file, int line);
#else
void* MemoryAlloc(size_t size);
void* MemoryRealloc(void* ptr, size_t size);
void  MemoryFree(void* ptr);
#endif

#define MemoryInit(dst, value, size)   memset(dst, value, size)
#define MemoryCopy(dst, src, size)     memcpy(dst, src, size)
#define MemoryMove(dst, src, size)     memmove(dst, src, size)

void MemoryDumpAllocs(void);
