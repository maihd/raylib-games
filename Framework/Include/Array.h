#pragma once

// ----------------------------------
// Internal functions
// ----------------------------------

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG
// INTERNAL
#define Array_NewMemory(capacity, elementSize) Array_NewMemoryDebug(capacity, elementSize, __FUNCTION__, __FILE__, __LINE__)

// INTERNAL
#define Array_FreeMemory(array) Array_FreeMemoryDebug(array, __FUNCTION__, __FILE__, __LINE__)

// INTERNAL
#define Array_GrowMemory(array, capacity, elementSize) Array_GrowMemoryDebug(array, capacity, elementSize, __FUNCTION__, __FILE__, __LINE__)

// INTERNAL
#define Array_MoveMemory(array, start, end, count, elementSize) Array_MoveMemoryDebug(array, start, end, count, elementSize, __FUNCTION__, __FILE__, __LINE__)

// INTERNAL
void*   Array_NewMemoryDebug(int capacity, int elementSize, const char* func, const char* file, int line);

// INTERNAL
int     Array_FreeMemoryDebug(void*  array, const char* func, const char* file, int line);

// INTERNAL
int     Array_GrowMemoryDebug(void** array, int capacity, int elementSize, const char* func, const char* file, int line);

// INTERNAL
int     Array_MoveMemoryDebug(void*  array, int start, int end, int count, int elementSize, const char* func, const char* file, int line);
#else
// INTERNAL
void*   Array_NewMemory(int capacity, int elementSize);

// INTERNAL
int     Array_FreeMemory(void*  array);

// INTERNAL
int     Array_GrowMemory(void** array, int capacity, int elementSize);

// INTERNAL
int     Array_MoveMemory(void*  array, int start, int end, int count, int elementSize);
#endif

#ifdef __cplusplus
}
#endif

// -----------------------------
// Inline functions
// -----------------------------

#define ARRAY_EMPTY                     0

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
