#include "Memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static inline uint32_t HashPtr32(void* ptr)
{
    const uint32_t magic = 2057;

    uint32_t value = (uint32_t)(uintptr_t)ptr;
    value = ~value + (value << 15);
    value = value ^ (value >> 12);
    value = value + (value << 2);
    value = value ^ (value >> 4);
    value = value * magic;
    value = value ^ (value >> 16);
    return value;
}

static inline uint64_t HashPtr64(void* ptr)
{
    const uint64_t magic = 41142057ULL;

    uint64_t value = (uint64_t)(uintptr_t)ptr;
    value = ~value + (value << 30);
    value = value ^ (value >> 24);
    value = value + (value << 4);
    value = value ^ (value >> 8);
    value = value * magic;
    value = value ^ (value >> 32);
    return value;
}

#if !defined(NDEBUG)

#if defined(__unix__)
#   include <unistd.h>
#elif defined(_WIN32)
#   include <windows.h>
#endif

typedef struct SysFreeList
{
    size_t  itemSize;
    void*   freeItem;
} SysFreeList;

typedef struct AllocDesc
{
    void*               ptr;
    size_t              size;

    const char*         func;
    const char*         file;
    int                 line;

    struct AllocDesc*   next;
} AllocDesc;

static void SysFreeListCollect(SysFreeList* freeList, void* item)
{
    *(void**)item = freeList->freeItem;
    freeList->freeItem = item;
}

static void* SysFreeListAcquire(SysFreeList* freeList)
{
    if (!freeList->freeItem)
    {
        const size_t itemSize = freeList->itemSize;
        const size_t allocSize = 64 * 1024;
        const size_t itemsPerBatch = allocSize / itemSize;

#if defined(__unix__)
        void* allocBatch = vmalloc(allocSize);
#elif defined(_WIN32)
        void* allocBatch = VirtualAlloc(NULL, (SIZE_T)allocSize, MEM_COMMIT, PAGE_READWRITE);
#else
        void* allocBatch = malloc(allocSize);
#endif

        for (size_t i = 0; i < itemsPerBatch; i++)
        {
            SysFreeListCollect(freeList, (uint8_t*)allocBatch + i * itemSize);
        }
    }

    void* result = freeList->freeItem;
    freeList->freeItem = *((void**)result);
    return result;
}

enum { ALLOC_DESC_COUNT = 64 };

static struct
{
    size_t      allocSize;
    int         allocCount;

    SysFreeList freeAllocDescs;
    AllocDesc*  hashAllocDescs[ALLOC_DESC_COUNT];
} Tracker = { 0, 0, { sizeof(AllocDesc), NULL } };

static void RegisterAlloc(void* ptr, size_t size, const char* func, const char* file, int line)
{
    AllocDesc* allocDesc = (AllocDesc*)SysFreeListAcquire(&Tracker.freeAllocDescs);

    allocDesc->ptr = ptr;
    allocDesc->size = size;
    allocDesc->func = func;
    allocDesc->file = file;
    allocDesc->line = line;

    uint64_t ptrHash = HashPtr64(ptr) & (ALLOC_DESC_COUNT - 1);
    allocDesc->next = Tracker.hashAllocDescs[ptrHash];
    Tracker.hashAllocDescs[ptrHash] = allocDesc;

    Tracker.allocSize += size;
    Tracker.allocCount++;
}

static void UpdateAlloc(void* ptr, void* newPtr, size_t size, const char* func, const char* file, int line)
{
    uint64_t ptrHash = HashPtr64(ptr) & (ALLOC_DESC_COUNT - 1);

    AllocDesc* prevAllocDesc = NULL;
    AllocDesc* allocDesc = Tracker.hashAllocDescs[ptrHash];
    while (allocDesc != NULL && allocDesc->ptr != ptr)
    {
        prevAllocDesc = allocDesc;
        allocDesc = allocDesc->next;
    }

    assert(allocDesc != NULL);
    allocDesc->ptr = newPtr;
    allocDesc->size = size;

    Tracker.allocSize -= allocDesc->size;
    Tracker.allocSize += size;

    uint64_t newPtrHash = HashPtr64(newPtr) & (ALLOC_DESC_COUNT - 1);
    if (newPtrHash != ptrHash)
    {
        if (prevAllocDesc)
        {
            prevAllocDesc->next = allocDesc->next;
        }
        else
        {
            Tracker.hashAllocDescs[ptrHash] = allocDesc->next;
        }

        allocDesc->next = Tracker.hashAllocDescs[newPtrHash];
        Tracker.hashAllocDescs[newPtrHash] = allocDesc;
    }
}

static void UnregisterAlloc(void* ptr, const char* func, const char* file, int line)
{
    uint64_t ptrHash = HashPtr64(ptr) & (ALLOC_DESC_COUNT - 1);

    AllocDesc* prevAllocDesc = NULL;
    AllocDesc* allocDesc = Tracker.hashAllocDescs[ptrHash];
    while (allocDesc != NULL && allocDesc->ptr != ptr)
    {
        prevAllocDesc = allocDesc;
        allocDesc = allocDesc->next;
    }
    assert(allocDesc != NULL);
    SysFreeListCollect(&Tracker.freeAllocDescs, allocDesc); 
    
    Tracker.allocSize -= allocDesc->size;
    Tracker.allocCount--;

    if (prevAllocDesc)
    {
        prevAllocDesc->next = allocDesc->next;
    }
    else
    {
        Tracker.hashAllocDescs[ptrHash] = allocDesc->next;
    }
}

void* MemoryAllocDebug(size_t size, const char* func, const char* file, int line)
{
    void* ptr = malloc(size);
    RegisterAlloc(ptr, size, func, file, line);
    return ptr;
}

void* MemoryReallocDebug(void* ptr, size_t size, const char* func, const char* file, int line)
{
    if (ptr == NULL)
    {
        return MemoryAllocDebug(size, func, file, line);
    }

    void* newPtr = realloc(ptr, size);
    UpdateAlloc(ptr, newPtr, size, func, file, line);
    return newPtr;
}

void MemoryFreeDebug(void* ptr, const char* func, const char* file, int line)
{
    if (ptr != NULL)
    {
        UnregisterAlloc(ptr, func, file, line);
        free(ptr);
    }
}

void MemoryDumpAllocs(void)
{
    printf("Address,Size,Source\n");
    for (int i = 0; i < ALLOC_DESC_COUNT; i++)
    {
        AllocDesc* allocDesc = Tracker.hashAllocDescs[i];
        while (allocDesc != NULL)
        {
            printf("0x%p,%zu,%s:%d:%s\n", allocDesc->ptr, allocDesc->size, allocDesc->file, allocDesc->line, allocDesc->func);
            allocDesc = allocDesc->next;
        }
    }
}
// END OF #if !defined(NDEBUG)
#else
void* MemoryAlloc(size_t size)
{
    return malloc(size);
}

void* MemoryRealloc(void* ptr, size_t size)
{
    return realloc(ptr, size);
}

void MemoryFree(void* ptr)
{
    free(ptr);
}

void MemoryDumpAllocs(void)
{
}
#endif
