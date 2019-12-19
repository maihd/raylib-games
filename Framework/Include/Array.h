#pragma once

void* ArrayGrow(void* array, int capacity, int elementSize);

#define ArrayNew(size)                  ((size) <= 0 ? 0 : ArrayGrow(0, size, 1));
#define ArrayFree(array)                (array ? (free((int*)(array) - 2), 0) : 0)

#define ArrayCount(array)               (array ? ((int*)(array) - 2)[0] : 0)
#define ArrayCapacity(array)            (array ? ((int*)(array) - 2)[1] : 0)

#define ArrayEnsure(array, capacity)    (ArrayCapacity(array) > (capacity) ? array : ArrayGrow(array, capacity, sizeof((array)[0])))  
#define ArrayResize(array, capacity)    ArrayGrow(array, capacity, sizeof((array)[0]))

#define ArrayPush(array, value)         ((array = ArrayEnsure(array, ArrayCount(array) + 1)) ? ((array)[((int*)(array) - 2)[0]++] = value, array) : 0)
#define ArrayPop(array)                 ((array)[--((int*)array - 2)[0]])
