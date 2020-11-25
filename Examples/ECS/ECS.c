#include "ECS.h"

#include <raylib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Debug.h>
#include <Array.h>
#include <Memory.h>
#include <FreeList.h>
#include <HashTable.h>

#define ECS_ENTITY_ID(index, life)  ((uint64_t)(life << 32) | index)
#define ECS_ENTITY_INDEX(entity)    ((uint32_t)((entity >> 32) << 32))
#define ECS_ENTITY_LIFE(entity)     ((uint32_t)(entity >> 32))

typedef struct
{
    void*                   data;
    int                     count;
    int                     capacity;
    int                     itemSize;

    Array(int)              freeIndexes;
} EcsBuffer;

EcsBuffer EcsBuffer_Create(int itemSize)
{
    EcsBuffer result = {
        .data = NULL,
        .count = 0,
        .capacity = 0,
        .itemSize = itemSize,
        .freeIndexes = ArrayNew(int, 0)
    };

    return result;
}

void EcsBuffer_Destroy(EcsBuffer* buffer)
{
    MemoryFree(buffer->data);
    ArrayFree(buffer->freeIndexes);

    buffer->data = NULL;
    buffer->count = 0;
    buffer->itemSize = 0;
}

int EcsBuffer_AddItem(EcsBuffer* buffer, const void* data)
{
    DebugAssert(buffer != NULL, "Your buffer is nullptr");

    if (ArrayCount(buffer->freeIndexes) > 0)
    {
        int index = ArrayPop(buffer->freeIndexes);
        memcpy((uint8_t*)buffer->data + index * buffer->itemSize, data, buffer->itemSize);
        return index;
    }
    else
    {
        if (buffer->count + 1 > buffer->capacity)
        {
            buffer->capacity = (buffer->capacity > 0 ? buffer->capacity * 2 : 8);
            buffer->data = MemoryRealloc(buffer->data, buffer->capacity * buffer->itemSize);
            DebugAssert(buffer->data != NULL, "Out of memory");
        }

        int index = buffer->count;
        memcpy((uint8_t*)buffer->data + index * buffer->itemSize, data, buffer->itemSize);
        buffer->count++;
        return index;
    }
}

void EcsBuffer_RemoveItem(EcsBuffer* buffer, int index)
{
    DebugAssert(buffer != NULL, "Your buffer is nullptr");
    DebugAssert(index > -1 && index < buffer->count, "index is out of range");

    buffer->count--;
    if (index != buffer->count - 1)
    {
        ArrayPush(buffer->freeIndexes, index);
    }
}

void* EcsBuffer_GetItem(EcsBuffer* buffer, int index)
{
    DebugAssert(buffer != NULL, "Require valid buffer");
    DebugAssert(index > -1 && index < buffer->count, "Index is out of range");

    return (char*)buffer->data + buffer->itemSize * index;
}

void EscBuffer_SetItem(EcsBuffer* buffer, int index, void* data)
{
    DebugAssert(buffer != NULL, "Require valid buffer");
    DebugAssert(index > -1 && index < buffer->count, "Index is out of range");

    memset((char*)buffer->data + buffer->itemSize * index, data, buffer->itemSize);
}

FreeListStruct(int);

struct EcsWorld
{
    FreeList(int)               entities;
    Array(int)                  entityLifes;

    HashTable(EcsBuffer)        componentsPool;
    HashTable(Array(EcsSystem)) systems;
};

EcsWorld* EcsWorld_Create(void)
{
    EcsWorld* world = MemoryAlloc(sizeof(*world));

    world->entities = FreeListNew(int, 0);

    HashTable_Init(world->systems, 64, 64);
    HashTable_Init(world->componentsPool, 64, 64);
    return world;
}

void EcsWorld_Destroy(EcsWorld* world)
{

}

int EcsWorld_NewEntity(EcsWorld* world)
{
    return -1;
}

void* EcsWorld_GetComponent(EcsWorld* world, int entity, int componentName)
{
    int index = ECS_ENTITY_INDEX(entity);
    int life = ECS_ENTITY_LIFE(entity);
    if (world->entityLifes[index] > life)
    {
        return NULL;
    }


    return NULL;
}

void* EcsWorld_AddComponent(EcsWorld* world, int entity, int componentName, void* data)
{
}

void EcsWorld_AddSystem(EcsWorld* world, int systemGroup, EcsSystem system)
{
    Array(EcsSystem) systemContainer;
    HashTable_GetValue(world->systems, systemGroup, ARRAY_EMPTY, &systemContainer);

    ArrayPush(systemContainer, system);
    HashTable_SetValue(world->systems, systemGroup, systemContainer);
}

void EcsWorld_ExecuteSystems(EcsWorld* world, int systemGroup)
{
    Array(EcsSystem) systemContainer;
    HashTable_GetValue(world->systems, systemGroup, ARRAY_EMPTY, &systemContainer);

    for (int i = 0, n = ArrayCount(systemContainer); i < n; i++)
    {
        EcsSystem system = systemContainer[i];
        DebugAssert(system, "Internal error: system mustnot be null");

        system(world);
    }
}

static float x = 0;

enum
{
    ECS_COMPONENT_TRANSFORM,
    ECS_COMPONENT_CIRCLE,
    ECS_COMPONENT_RECTANGLE,
};

void UpdateSystem(EcsWorld* world)
{
    float dt = GetFrameTime();

    Array(int) lifes = world->entityLifes;
    Array(int) entities = world->entities.elements;
    
    for (int i = 0, n = FreeListCount(world->entities); i < n; i++)
    {
        int entity = entities[i];
        if (ECS_ENTITY_LIFE(entity) == lifes[i])
        {
            Vector2* transform = EcsWorld_GetComponent(world, entity, ECS_COMPONENT_TRANSFORM);
            if (transform != NULL)
            {
                transform->x = 60 * dt;
            }
        }
    }
    x += dt * 60;
}

void RenderSystem(EcsWorld* world)
{
    DrawCircle((int)x, 300, 30, BLACK);
}

int main()
{
    InitWindow(800, 600, "ECS");

    EcsWorld* world = EcsWorld_Create();

    EcsWorld_AddSystem(world, ECS_SYSTEM_UPDATE, UpdateSystem);
    EcsWorld_AddSystem(world, ECS_SYSTEM_RENDER, RenderSystem);

    while (!WindowShouldClose())
    {
        EcsWorld_ExecuteSystems(world, ECS_SYSTEM_UPDATE);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
        
            EcsWorld_ExecuteSystems(world, ECS_SYSTEM_RENDER);
        }
        EndDrawing(); 
    }

    CloseWindow();
    return 0;
}