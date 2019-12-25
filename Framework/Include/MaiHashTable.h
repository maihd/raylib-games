#pragma once

#include "./MaiTypes.h"

struct HashTableS
{
    int     count;
    int     capacity;
    int     hashCount;
    int*    hashs;
    int*    nexts;
    u64*    keys;
};

#define HashTableHeader(table)      ((struct HashTableS*)(table) - 1)
#define hashtableCount(table)       ((table) ? HashTableHeader(table)->count : 0)
#define hashtableCapacity(table)    ((table) ? HashTableHeader(table)->capacity : 0)

#define HashTableNew(T, hashCount, capacity)                                        \

#define hashtable_init(t, HASH_COUNT)                                               \
    do {                                                                            \
        struct HashTableS* _t = (struct HashTableS*)malloc(sizeof(struct HashTableS)); \
        _t->size  = 0;                                                              \
        _t->count = 0;                                                              \
        _t->hash_count = HASH_COUNT;                                                \
        _t->hashs = (int*)malloc(sizeof(int) * (HASH_COUNT));                       \
        _t->nexts = NULL;                                                           \
        _t->keys  = NULL;                                                           \
                                                                                    \
        HASHTABLE_ASSERT(_t->hashs, "Hash list is invalid!");                       \
        int _i, _n;                                                                 \
        for (_i = 0, _n = (HASH_COUNT); _i < _n; _i++) {                            \
            _t->hashs[_i] = -1;                                                     \
        }                                                                           \
                                                                                    \
        *(void**)&(t) = _t + 1;                                                     \
    } while (0)

#define hashtable_free(t)                           \
    do {                                            \
        struct hashtable* __raw = hashtable_raw(t); \
        free(__raw->keys);                          \
        free(__raw->nexts);                         \
        free(__raw->hashs);                         \
        free(__raw);                                \
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
