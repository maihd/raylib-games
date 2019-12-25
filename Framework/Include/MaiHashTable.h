#pragma once

#include "./MaiTypes.h"
                   
#define HashTable(T, name)  \
    struct name             \
    {                       \
        int     count;      \
        int     capacity;   \
        int     hashCount;  \
        int*    hashs;      \
        int*    nexts;      \
        u64*    keys;       \
        T*      values;     \
    }

#define HashTableInit(table, HASH_COUNT, CAPACITY)                                      \
    do {                                                                                \
        (table).count = 0;                                                              \
        (table).capacity = CAPACITY;                                                    \
        (table).hashCount = HASH_COUNT;                                                 \
        (table).hashs = CAPACITY > 0 ? (int*)malloc(sizeof(int) * (HASH_COUNT)) : 0;    \
                                                                                        \
        if ((table).hashs) {                                                            \
            int I, N;                                                                   \
            for (I = 0, N = (HASH_COUNT); I < N; I++) {                                 \
                (table).hashs[I] = -1;                                                  \
            }                                                                           \
        }                                                                               \
                                                                                        \
        if (CAPACITY > 0) {                                                             \
            int cellSize = sizeof(int) + sizeof(u64) + sizeof((table).values[0]);       \
            void* buffer = malloc(CAPACITY * cellSize);                                 \
            if (buffer) {                                                               \
                (table).nexts  = (int*)buffer;                                          \
                (table).keys   = (u64*)((table).nexts + CAPACITY);                      \
                (table).values = (T*)((table).keys + CAPACITY);                         \
                break;                                                                  \
            }                                                                           \
        }                                                                               \
                                                                                        \
        (table).nexts  = 0;                                                             \
        (table).keys   = 0;                                                             \
        (table).values = 0;                                                             \
    } while (0)

#define HashTableFree(t)                        \
    do {                                        \
        free((t).values);                       \
        free((t).keys);                         \
        free((t).nexts);                        \
        free((t).hashs);                        \
        (t).values = 0;                         \
        (t).keys = 0;                           \
        (t).nexts = 0;                          \
        (t).hashs = 0;                          \
        (t).count = 0;                          \
        (t).capacity = 0;                       \
    } while (0)

#define hashtable_set(t, key, value)                        \
    do {                                                    \
        int __h, __p;                                       \
        int __c = hashtable_find(t, key, &__h, &__p);       \
        if (__c > -1) {                                     \
            t[__c] = value;                                 \
        } else {                                            \
            int S = hashtable_count(t) + 1;                 \
            if (hashtable_ensure(t, S)) {                   \
                struct hashtable* __raw = hashtable_raw(t); \
                __c = __raw->count++;                       \
                if (__p > -1) {                             \
                    __raw->nexts[__p] = __c;                \
                } else {                                    \
                    __raw->hashs[__h] = __c;                \
                }                                           \
                                                            \
                __raw->nexts[__c]  = -1;                    \
                __raw->keys[__c]   = key;                   \
                (t)[__c] = value;                           \
            }                                               \
        }                                                   \
    } while (0)

#define hashtable_get(t, key, out_value)                    \
    do {                                                    \
        int __c = hashtable_find(t, key, NULL, NULL);       \
        if (__c > -1) {                                     \
            out_value = (t)[__c];                           \
        }                                                   \
    } while (0)

#define hashtable_has(t, key) (hashtable_find(t, key, NULL, NULL) > -1)

#define hashtable_ensure(t, n) (hashtable_size(t) < (n) ? (*(void**)&(t) = hashtable_grow(t, (n), sizeof((t)[0]))) != NULL : 1)
