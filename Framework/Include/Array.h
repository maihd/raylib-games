#pragma once

int ArrayGrow(void** array, int capacity, int elementSize);

#define Array(T)                        T*

#define ArrayNew(T, size)               (Array(T))(((size) <= 0 ? 0 : ArrayGrow(0, size, 1)))
#define ArrayFree(array)                (array = (array ? (free((int*)(array) - 2), 0) : 0))

#define ArrayCount(array)               (array ? ((int*)(array) - 2)[0] : 0)
#define ArrayCapacity(array)            (array ? ((int*)(array) - 2)[1] : 0)

#define ArrayEnsure(array, capacity)    ((ArrayCapacity(array) > (capacity) ? 1 : ArrayGrow(&(array), capacity, sizeof((array)[0]))))
#define ArrayResize(array, capacity)    (ArrayGrow(&(array), capacity, sizeof((array)[0])))

#define ArrayPush(array, value)         (ArrayEnsure(array, ArrayCount(array) + 1) ? ((array)[((int*)(array) - 2)[0]++] = value, 1) : 0)
#define ArrayPop(array)                 ((array)[--((int*)array - 2)[0]])
