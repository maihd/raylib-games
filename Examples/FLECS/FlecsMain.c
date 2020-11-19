#include "flecs.h"
#include <raylib.h>

typedef struct Circle
{
    float radius;
} Circle;

typedef struct Position
{
    float x;
    float y;
} Position;

typedef struct Velecity
{
    float x;
    float y;
} Velocity;

void Move(ecs_iter_t* iter)
{
    ECS_COLUMN(iter, Position, positions, 1);
    ECS_COLUMN(iter, Velocity, velocities, 2);

    for (int i = 0; i < iter->count; i++) {
        positions[i].x += velocities[i].x;
        positions[i].y += velocities[i].y;
    }
}

int main(int argc, char* argv[])
{
    ecs_world_t* world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Circle);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    ECS_ENTITY(world, circle, 0);

    ecs_set(world, circle, Position, { 10, 20 });
    ecs_set(world, circle, Velocity, { 1, 2 });

    InitWindow(800, 600, "ECS");

    SetTargetFPS(60);
    ecs_set_target_fps(world, 60);

    while (!WindowShouldClose())
    {
        ecs_progress(world, 0);

        BeginDrawing();
        {
            ClearBackground(DARKBLUE);
        }
        EndDrawing();
    }

    CloseWindow();
    return ecs_fini(world);
}