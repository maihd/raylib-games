#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// INTERNAL
void*    Array_NewMemory(int capacity, int elementSize);

// INTERNAL
int      Array_FreeMemory(void*  array);

// INTERNAL
int      Array_GrowMemory(void** array, int capacity, int elementSize);

// INTERNAL
int      Array_MoveMemory(void*  array, int start, int end, int count, int elementSize);

#ifdef __cplusplus
}
#endif

#ifndef Array
#define Array(T)                        T*
#endif

#define ArrayNew(T, capacity)           (Array(T))(((capacity) <= 0 ? 0 : Array_NewMemory(capacity, sizeof(T))))
#define ArrayFree(array)                if (Array_FreeMemory(array)) (array) = 0

#define ArrayCount(array)               (array ? ((int*)(array) - 2)[0] : 0)
#define ArrayCapacity(array)            (array ? ((int*)(array) - 2)[1] : 0)

#define ArraySetCount(array, count)     (array ? (((int*)(array) - 2)[0] = count) : 0)

#define ArrayClear(array)               if (array) ((int*)(array) - 2)[0] = 0

#define ArrayEnsure(array, capacity)    (ArrayCapacity(array) >= (capacity) ? 1 : Array_GrowMemory(&(array), capacity, sizeof((array)[0])))
#define ArrayResize(array, capacity)    (Array_GrowMemory((void**)(&(array)), capacity, sizeof((array)[0])))

#define ArrayPush(array, value)         (ArrayEnsure(array, ArrayCount(array) + 1) ? ((array)[((int*)(array) - 2)[0]++] = (value), 1) : 0)
#define ArrayPop(array)                 ((array)[--((int*)(array) - 2)[0]])

#define ArrayIndexOf(array, value, outIndex)                    \
    do {                                                        \
        outIndex = -1;                                          \
        for (int i = 0, n = ArrayCount(array); i < n; i++) {    \
            if (array[i] == value) {                            \
                outIndex = i;                                   \
                break;                                          \
            }                                                   \
        }                                                       \
    } while (0)

#define ArrayLastIndexOf(array, value, outIndex)                \
    do {                                                        \
        outIndex = -1;                                          \
        for (int i = 0, n = ArrayCount(array); i < n; i++) {    \
            if (array[i] == value) {                            \
                outIndex = i;                                   \
            }                                                   \
        }                                                       \
    } while (0)

#define ArrayErase(array, index)                                                                                \
    do {                                                                                                        \
        if (index > -1 && index < ArrayCount(array)) {                                                          \
            if (index < ArrayCount(array) - 1) {                                                                \
                Array_MoveMemory(array, index, index + 1, ArrayCount(array) - index - 1, sizeof((array)[0]));   \
            }                                                                                                   \
            ((int*)(array) - 2)[0]--;                                                                           \
        }                                                                                                       \
    } while (0)

#define ArrayEraseWithHole(array, index)                                                                    \
    do {                                                                                                    \
        if (index > -1 && index < ArrayCount(array)) {                                                      \
            if (index < ArrayCount(array) - 1) {                                                            \
                array[index] = array[ArrayCount(array) - 1];                                                \
            }                                                                                               \
            ((int*)(array) - 2)[0]--;                                                                       \
        }                                                                                                   \
    } while (0)

#define ArrayRemove(array, value)                                                                           \
    do {                                                                                                    \
        int index;                                                                                          \
        ArrayIndexOf(array, value, index);                                                                  \
        ArrayErase(array, index);                                                                           \
    } while (0)

#define ArrayRemoveLast(array, value)                                                                       \
    do {                                                                                                    \
        int index;                                                                                          \
        ArrayLastIndexOf(array, value, index);                                                              \
        ArrayErase(array, index);                                                                           \
    } while (0)

#define ArrayRemoveWithHole(array, value)                                                                   \
    do {                                                                                                    \
        int index;                                                                                          \
        ArrayIndexOf(array, value, index);                                                                  \
        ArrayErase(array, index);                                                                           \
    } while (0)

#define ArrayRemoveLastWithHole(array, value)                                                               \
    do {                                                                                                    \
        int index;                                                                                          \
        ArrayLastIndexOf(array, value, index);                                                              \
        ArrayErase(array, index);                                                                           \
    } while (0)
