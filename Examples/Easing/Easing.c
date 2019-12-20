#include <raylib.h>

int main(void)
{
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
   
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Circle Easing");

    SetTargetFPS(60);

    enum {
        MOVE, 
        INLARGE,
        FADING,
        END
    } state = MOVE;

    int frameCounter = 0;
    float ballPositionX = -SCREEN_WIDTH * 0.5f;
    float ballRadius = 20;
    float ballAlpha = 1.0f;
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            // Update 
            switch (state)
            {
            case MOVE:
                //ballPositionX = EaseElastic
                break;

            case INLARGE:
                break;

            case FADING:
                break;

            case END:
                break;
            }

            // Render
            switch (state)
            {
            case END:
                break;

            default:
                break;
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}