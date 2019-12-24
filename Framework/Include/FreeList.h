#pragma once

#include "./Array.h"

#define FreeList(T)                 struct FreeList_##T { Array(T) elements; Array(int) indices; }
#define FreeListNew(T, capacity)    (struct FreeList_##T){ ArrayNew(T, capacity), ArrayNew(int, 0) }
#define FreeListFree(list)          (ArrayFree((list).elements), ArrayFree((list).indices))

#define FreeListCount(list)         (ArrayCount((list).elements) - ArrayCount((list).indices))
#define FreeListCapacity(list)      (ArrayCapacity((list).elements))

#define FreeListAdd(list, value)                    \
    do {                                            \
        if (ArrayCount((list).indices) > 0) {       \
            int index = ArrayPop((list).indices);   \
            (list).elements[index] = value;         \
        } else {                                    \
            ArrayPush((list).elements, value);      \
        }                                           \
    } while (0)

#define FreeListErase(list, index)                  \
    ArrayPush((list).indices, index)

#define FreeListRemove(list, value)                 \
    do {                                            \
        int index = 0;                              \
        ArrayIndexOf((list).elements, value, index);\
        if (index > -1) {                           \
            ArrayPush((list).indices, index);       \
        }                                           \
    } while (0)
