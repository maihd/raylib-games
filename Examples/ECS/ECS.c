#include <raylib.h>

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