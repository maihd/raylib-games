#include <Array.h>

#include <Debug.h>
#include <Memory.h>

#include <string.h>

typedef struct
{
    int count;
    int capacity;
} ArrayHeader;

void* Array_NewMemory(int capacity, int elementSize)
{
    DebugAssert(capacity >= 0, "capacity should not be negative: capacity=%d", capacity);
    DebugAssert(elementSize > 0, "elementSize must be positive: elementSize=%d", elementSize);

    int newCapacity = (capacity > 16) ? capacity - 1 : 15;
    newCapacity = newCapacity | (newCapacity >> 1);
    newCapacity = newCapacity | (newCapacity >> 2);
    newCapacity = newCapacity | (newCapacity >> 4);
    newCapacity = newCapacity | (newCapacity >> 8);
    newCapacity = newCapacity | (newCapacity >> 16);
    newCapacity = newCapacity + 1;

    ArrayHeader* newBuffer = MemoryAlloc(sizeof(ArrayHeader) + newCapacity * elementSize);
    if (newBuffer)
    {
        newBuffer->count    = 0;
        newBuffer->capacity = newCapacity;
    }

    return newBuffer + 1;
}

int Array_FreeMemory(void* array)
{
    if (array)
    {
        MemoryFree((ArrayHeader*)array - 1);
        return 1;
    }
    else
    {
        return 0;
    }
}

int Array_GrowMemory(void** array, int capacity, int elementSize)
{
    DebugAssert(array != NULL, "array must be not null");
    DebugAssert(capacity >= 0, "capacity should not be negative: capacity=%d", capacity);
    DebugAssert(elementSize > 0, "elementSize must be positive: elementSize=%d", elementSize);
    
    int newCapacity = (capacity > 16) ? capacity - 1 : 15;
    newCapacity = newCapacity | (newCapacity >> 1);
    newCapacity = newCapacity | (newCapacity >> 2);
    newCapacity = newCapacity | (newCapacity >> 4);
    newCapacity = newCapacity | (newCapacity >> 8);
    newCapacity = newCapacity | (newCapacity >> 16);
    newCapacity = newCapacity + 1;

    int oldCount = ArrayCount(*array);
    ArrayHeader* oldBuffer = *array ? ((ArrayHeader*)(*array) - 1) : NULL;
    ArrayHeader* newBuffer = MemoryRealloc(oldBuffer, sizeof(ArrayHeader) + newCapacity * elementSize);

    if (newBuffer)
    {
        newBuffer->count    = oldCount;
        newBuffer->capacity = newCapacity;

        *array = (newBuffer + 1);

        return 1;
    }
    else
    {
        return 0;
    }
}

int Array_MoveMemory(void* array, int start, int end, int count, int elementSize)
{
    DebugAssert(array != NULL, "array must be not null");
    DebugAssert(elementSize > 0, "elementSize must be positive: elementSize=%d", elementSize);

    if (array && count > 0)
    {
        return memmove((char*)array + start * elementSize, (char*)array + end * elementSize, count * elementSize) == array;
    }
    else
    {
        return 0;
    }
}
