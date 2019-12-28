#include <MaiLib.h>

int main(void)
{
    int screenWidth = 1280;
    int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Slime Slayer 2D");

    while (!PollWindowEvents())
    {
        
        BeginDrawing();
        {
            ClearBackground(BLACK);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}