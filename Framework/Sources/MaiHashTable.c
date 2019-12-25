#include <MaiHashTable.h>

#include <assert.h>
#include <stdlib.h>

#if 0
int HashTableFind(void* table, u64 key, int* out_hash, int* out_prev)
{
    struct hashtable* raw = hashtable_raw(table);        
    int hash = (key) % raw->hash_count;            
    int curr = raw->hashs[hash];                 
    int prev = -1;                                   
                                        
    while (curr > -1) 
    {                              
        if (raw->keys[curr] == key) 
        {              
            break;                                     
        }                                              
        prev = curr; 
        curr = raw->nexts[curr];
    }                                                  
                                        
    if (out_prev) *out_prev = prev;                
    if (out_hash) *out_hash = hash;   
    return curr;                                 
}

void* hashtable_grow(void* table, int reqsize, int elemsize)
{
    if (!table)
    {
        return NULL;
    }

    struct hashtable* raw = hashtable_raw(table);
    int size = raw->size > 0 ? raw->size : 8;
    while (size < reqsize) size *= 2;

    int* keys = raw->keys;
    int* nexts = raw->nexts;
    int* hashs = raw->hashs;
    
    struct hashtable* new_table = (struct hashtable*)realloc(raw, sizeof(struct hashtable) + size * elemsize);
    if (new_table)
    {
        new_table->size  = size;
        new_table->nexts = (int*)realloc(new_table->nexts, size * sizeof(int));
        new_table->keys  = (int*)realloc(new_table->keys, size * sizeof(int));

        if (!new_table->nexts || !new_table->keys)
        {
            free(new_table->keys);
            free(new_table->nexts);
            free(new_table->hashs);
            free(new_table);
            return NULL;
        }
        else
        {
            return new_table + 1;
        }
    }
    else
    {
        free(keys);
        free(nexts);
        free(hashs);
        return NULL;
    }
}
#endif
