#pragma once

#include "./Array.h"

#define FreeListStruct(T)           FreeList(T) { Array(T) elements; Array(int) freeElements; }

#define FreeList(T)                 struct FreeList_##T
#define FreeListNew(T, capacity)    (FreeList(T)){ ArrayNew(T, capacity), ArrayNew(int, capacity) }
#define FreeListFree(list)          ArrayFree((list).elements); ArrayFree((list).freeElements)

#define FreeListCount(list)         (ArrayCount((list).elements))
#define FreeListCapacity(list)      (ArrayCapacity((list).elements))

#define FreeListRef(list, index)            (&(list).elements[index])
#define FreeListGet(list, index)            ((list).elements[index])
#define FreeListSet(list, index, value)     ((list).elements[index] = value)

#define FreeListClear(list)                 ArrayClear((list).elements); ArrayClear((list).freeElements)

#define FreeListCollect(list, index)                        if (index > -1 && index < ArrayCount((list).elements)) ArrayPush((list).freeElements, index)

#define FreeListAdd(list, value)                            \
    do {                                                    \
        if (ArrayCount((list).freeElements) > 0) {          \
            int index = ArrayPop((list).freeElements);      \
            (list).elements[index] = value;                 \
        } else {                                            \
            ArrayPush((list).elements, value);              \
        }                                                   \
    } while (0)

#define FreeListRemove(list, value)                         \
    do {                                                    \
        int index = 0;                                      \
        ArrayIndexOf((list).elements, value, index);        \
        FreeListCollect(list, index);                       \
    } while (0)

