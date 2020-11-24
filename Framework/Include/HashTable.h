#pragma once

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifndef HASHTABLE_ASSERT
#define HASHTABLE_ASSERT(exp, msg) assert((exp) && (msg))
#endif

/// struct HashTable
/// types:              int    int       int          int*          int*       unsigned*        T*
/// memory layout:   |[size]|[count]|[hashCount]|[---hashs---]|[---nexts---]|[---keys---]|[---values---]|
/// memory offset: -12     -8      -4           0         size*4        size*8       size*12        size*(12 + sizeof(T))
#define HashTable(T)                T*

#define HashTable_GetMeta(table)        ((int*)(table) - 3)
#define HashTable_GetSize(table)        ((table) ? HashTable_GetMeta(table)[0] : 0)
#define HashTable_GetCount(table)       ((table) ? HashTable_GetMeta(table)[1] : 0)
#define HashTable_GetHashCount(table)   ((table) ? HashTable_GetMeta(table)[2] : 0)

#define HashTable_CalcBufferSize(size, hashCount, itemSize) (3 * sizeof(int) + (size) * (sizeof(int) + sizeof(unsigned) + (itemSize)) + (hashCount) * sizeof(int))

#define HashTable_Init(table, size, hashCount)                                                      \
    do {                                                                                            \
        HASHTABLE_ASSERT((size) > 0, "size must be > 0");                                           \
        HASHTABLE_ASSERT((hashCount) > 0, "hashCount must be > 0");                                 \
                                                                                                    \
        int* meta = malloc(HashTable_CalcBufferSize(size, hashCount, sizeof((table)[0])));          \
        meta[0]   = size;                                                                           \
        meta[1]   = 0;                                                                              \
        meta[2]   = hashCount;                                                                      \
                                                                                                    \
        int i, n;                                                                                   \
        int* hashs = meta + 3;                                                                      \
        for (i = 0, n = (hashCount); i < n; i++) {                                                  \
            hashs[i] = -1;                                                                          \
        }                                                                                           \
                                                                                                    \
        *(void**)&(table) = hashs;                                                                  \
    } while (0)

#define HashTable_Free(table)                       \
    do {                                            \
        if (table) {                                \
            int* meta = HashTable_GetMeta(table);   \
            free(meta);                             \
            (table) = 0;                            \
        }                                           \
    } while (0)

#define HashTable_Ensure(table, n) (HashTable_GetSize(table) < (n) ? (*(void**)&(table) = HashTable_Grow(table, (n), sizeof((table)[0]))) != NULL : 1)

#define HashTable_SetValue(t, key, value)                                                       \
    do {                                                                                        \
        int hash, prev;                                                                         \
        int curr = HashTable_Find(t, key, &hash, &prev);                                        \
        if (curr > -1) {                                                                        \
            void* values = ((int*)(t)) + HashTable_GetHashCount(t) + 2 * HashTable_GetSize(t);  \
            void* ptr = t;                                                                      \
            *(void**)&(t) = values;                                                             \
            (t)[curr] = value;                                                                  \
            *(void**)&(t) = ptr;                                                                \
        } else {                                                                                \
            if (HashTable_Ensure(t, HashTable_GetCount(t) + 1)) {                               \
                int* raw = HashTable_GetMeta(t);                                                \
                                                                                                \
                int  size      = raw[0];                                                        \
                int  hashCount = raw[2];                                                        \
                                                                                                \
                int* hashs = (int*)(t);                                                         \
                int* nexts = hashs + hashCount;                                                 \
                int* keys  = nexts + size;                                                      \
                                                                                                \
                curr = raw[1]++;                                                                \
                if (prev > -1) {                                                                \
                    nexts[prev] = curr;                                                         \
                } else {                                                                        \
                    hashs[hash] = curr;                                                         \
                }                                                                               \
                                                                                                \
                nexts[curr]  = -1;                                                              \
                keys[curr]   = key;                                                             \
                                                                                                \
                void* values = keys + size;                                                     \
                void* ptr = t;                                                                  \
                *(void**)&(t) = values;                                                         \
                (t)[curr] = value;                                                              \
                *(void**)&(t) = ptr;                                                            \
            }                                                                                   \
        }                                                                                       \
    } while (0)

#define HashTable_GetValue(table, key, defValue, outValue)                                                  \
    do {                                                                                                    \
        HASHTABLE_ASSERT(outValue != 0, "outValue must not be null");                                       \
                                                                                                            \
        int currIndex = HashTable_Find(table, key, NULL, NULL);                                             \
        if (currIndex > -1) {                                                                               \
            void* values = ((int*)(table)) + HashTable_GetHashCount(table) + 2 * HashTable_GetSize(table);  \
            void* ptr = table;                                                                              \
            *(void**)&(table) = values;                                                                     \
            *(outValue) = (table)[currIndex];                                                               \
            *(void**)&(table) = ptr;                                                                        \
        } else {                                                                                            \
            *(outValue) = defValue;                                                                         \
        }                                                                                                   \
    } while (0)

#define HashTable_HasKey(table, key) (HashTable_Find(table, key, NULL, NULL) > -1)

static int HashTable_Find(void* table, unsigned key, int* outHashIndex, int* outPrevIndex)
{
    if (!table)
    {
        return -1;
    }

    int* meta = HashTable_GetMeta(table);
    int  size = meta[0];
    int  hashCount = meta[2];

    int*        hashs = (int*)table;
    int*        nexts = hashs + hashCount;
    unsigned*   keys = (unsigned*)(nexts + size);

    int  hashIndex = key % hashCount;

    int  currIndex = hashs[hashIndex];
    int  prevIndex = -1;

    while (currIndex > -1)
    {
        if (keys[currIndex] == key)
        {
            break;
        }

        prevIndex = currIndex;
        currIndex = nexts[currIndex];
    }

    if (outPrevIndex) *outPrevIndex = prevIndex;
    if (outHashIndex) *outHashIndex = hashIndex;
    return currIndex;
}

static void* HashTable_Grow(void* table, int targetSize, int itemSize)
{
    if (!table)
    {
        return NULL;
    }

    int* meta = HashTable_GetMeta(table);

    int oldSize = meta[0];
    int newSize = (oldSize > 16 ? oldSize : 16); while (newSize < targetSize) newSize *= 2;
    if (oldSize == newSize)
    {
        return table;
    }

    int oldCount = meta[1];
    int newCount = oldCount;

    int hashCount = meta[2];

    int* newMeta = (int*)realloc(meta, HashTable_CalcBufferSize(newSize, hashCount, itemSize));
    if (newMeta)
    {
        if (oldSize > 0)
        {
            int*        oldHashs = newMeta + 2;
            int*        newHashs = newMeta + 2;

            int*        oldNexts = oldHashs + hashCount;
            int*        newNexts = newHashs + hashCount;

            unsigned*   oldKeys = (unsigned*)(oldNexts + oldSize);
            unsigned*   newKeys = (unsigned*)(newNexts + newSize);

            void*       oldValues = oldKeys + oldSize;
            void*       newValues = newKeys + newSize;

            memmove(newValues, oldValues, oldSize * itemSize);
            memmove(newKeys, oldKeys, oldSize * sizeof(int));
        }

        newMeta[0] = newSize;
        newMeta[1] = newCount;
        return newMeta + 3;
    }
    else
    {
        free(meta);
        return NULL;
    }
}

