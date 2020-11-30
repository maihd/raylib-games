#pragma once

#include <raylib.h>

#include <Array.h>
#include <FreeList.h>

typedef struct Entity
{
    bool    active;

    float   scale;
    float   rotation;
    Vector2 position;

    Vector2 velocity;
    float   movespeed;

    Color   color;
    float   radius;

    Texture texture;
} Entity;

FreeListStruct(Entity);

typedef struct PointMass
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float invMass;
    float damping;
} PointMass;

typedef struct Spring
{
    PointMass* p0;
    PointMass* p1;

    float targetLength;
    float stiffness;
    float damping;
    float force;
} Spring;

typedef struct WarpGrid
{
    int cols;
    int rows; 

    Array(Spring)    springs;

    Array(PointMass) points;
    Array(PointMass) fixedPoints;
} WarpGrid;

typedef struct World
{
    WarpGrid grid;

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

void    WorldUpdate(World* world, float horizontalInput, float verticalInput, Vector2 aimDir, bool fire, float deltaTime);
void    WorldRender(World world);

