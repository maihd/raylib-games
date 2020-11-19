#include "ECS.h"

#include <raylib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Array.h>
#include <Memory.h>

#define ECS_ENTITY_ID(index, life)  ((uint64_t)(life << 32) | index)
#define ECS_ENTITY_INDEX(entity)    ((uint32_t)((entity >> 32) << 32))
#define ECS_ENTITY_LIFE(entity)     ((uint32_t)(entity >> 32))

typedef struct
{
    void*                   data;
    int                     count;
    
    int                     itemSize;

    Array(int)              indexes;
} EcsBuffer;

struct EcsWorld
{
    int                 maxEntities;
    int                 componentCount;
    int                 systemCount;

    int                 maxIndex;

    int*                lifes;
    int*                indexes;
    
    EcsSystem*          systems;
    int*                components;

    EcsComponentBool*   pool;
    int                 systemTop;
};

int main()
{
    InitWindow(800, 600, "ECS");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
        }
        EndDrawing(); 
    }

    CloseWindow();
    return 0;
}