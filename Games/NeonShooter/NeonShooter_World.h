#pragma once

#include <MaiLib.h>
#include <MaiArray.h>
#include <MaiFreeList.h>

typedef struct Entity
{
    bool    active;

    float   scale;
    float   rotation;
    vec2    position;

    vec2    velocity;
    float   movespeed;

    Color   color;
    float   radius;

    Texture texture;
} Entity;

FreeListStruct(Entity);

typedef struct World
{
    Entity          player;

    FreeList(Entity)   bullets;
    FreeList(Entity)   seekers;
    FreeList(Entity)   wanderers;
    FreeList(Entity)   blackHoles;

    int             seekerSpawnRate;
    int             wandererSpawnRate;
    int             blackHoleSpawnRate;

    bool            oldFire;
    float           fireTimer;
    float           fireInterval;

    float           spawnTimer;
    float           spawnInterval;

    bool            lock;
    float           gameOverTimer;
} World;

World   WorldNew(void);
void    WorldFree(World* world);

void    WorldUpdate(World* world, float horizontalInput, float verticalInput, vec2 aimDir, bool fire, float deltaTime);
void    WorldRender(World world);

