#pragma once

#include <stdlib.h>

#define MemoryAlloc(size)           malloc(size)
#define MemoryRealloc(ptr, size)    realloc(ptr, size)
#define MemoryFree(ptr)             free(ptr)
