#include "flecs.h"

#include <math.h>
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
    float dt = GetFrameTime();

    ECS_COLUMN(iter, Position, positions, 1);
    ECS_COLUMN(iter, Velocity, velocities, 2);

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    for (int i = 0; i < iter->count; i++)
    {
        positions[i].x += velocities[i].x * dt;
        positions[i].y += velocities[i].y * dt;

        if (positions[i].x < 0)
        {
            velocities[i].x = fabsf(velocities[i].x);
        }
        
        if (positions[i].x > width)
        {
            velocities[i].x = -fabsf(velocities[i].x);
        }

        if (positions[i].y < 0)
        {
            velocities[i].y = fabsf(velocities[i].y);
        }
        
        if (positions[i].y > height)
        {
            velocities[i].y = -fabsf(velocities[i].y);
        }
    }
}

void Render(ecs_iter_t* iter)
{
    ECS_COLUMN(iter, Position, positions, 1);
    ECS_COLUMN(iter, Circle, circles, 2);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (int i = 0; i < iter->count; i++)
    {
        DrawCircle(positions[i].x, positions[i].y, circles[i].radius, BLACK);
    }
    DrawRectangle(0, 0, 90, 35, WHITE);
    DrawFPS(10, 10);
    EndDrawing();
}

int main(int argc, char* argv[])
{
    ecs_world_t* world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Circle);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
    ECS_SYSTEM(world, Render, EcsOnUpdate, Position, Circle);

    int width = 800;
    int height = 600;

    for (int i = 0; i < 3000; i++)
    {
        ecs_entity_t prefab = 0;
        ecs_entity_t circle = ecs_new(world, Circle);

        ecs_set(world, circle, Position, { GetRandomValue(0, width), GetRandomValue(0, height) });
        ecs_set(world, circle, Velocity, { GetRandomValue(-width / 2, width / 2), GetRandomValue(-height / 2, height / 2) });
        ecs_set(world, circle, Circle, { 10.0f });
    }

    InitWindow(width, height, "ECS");

    SetTargetFPS(60);
    ecs_set_target_fps(world, 60);

    while (!WindowShouldClose())
    {
        ecs_progress(world, GetFrameTime());
    }

    CloseWindow();
    return ecs_fini(world);
}