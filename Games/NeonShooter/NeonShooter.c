#include <MaiLib.h>

#include "NeonShooter_ParticleSystem.h"
#include "NeonShooter_World.h"

int main(void)
{
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Neon shooter");

    SetTargetFPS(60);

    World world = WorldNew();

    while (!WindowShouldClose())
    {
        WorldUpdate(&world, 0, 0, (vec2) { 0, 0 }, false, GetFrameTime());

        BeginDrawing();
        {
            ClearBackground(BLACK);

            WorldRender(world);
        }
        EndDrawing();
    }

    WorldFree(&world);
    CloseWindow();
    return 0;
}