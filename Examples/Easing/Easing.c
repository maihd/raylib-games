#include <MaiLib.h>
#include <MaiEasings.h>

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

    float timer = 0;
    float ballPositionX = -SCREEN_WIDTH * 0.5f;
    float ballRadius = 20;
    float ballAlpha = 1.0f;
    
    while (!WindowShouldClose())
    {
        timer += GetFrameTime();

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            // Update 
            switch (state)
            {
            case MOVE:
                ballPositionX = EaseElasticOut(-SCREEN_WIDTH * 0.5f, SCREEN_WIDTH * 0.5f, timer, 2.0f);
                if (timer >= 2.0f)
                {
                    timer = 0;
                    state = INLARGE;
                }
                break;

            case INLARGE:
                ballRadius = EaseElasticIn(20, SCREEN_WIDTH, timer, 3.0f);
                if (timer >= 3.0f)
                {
                    timer = 0.0f;
                    state = FADING;
                }
                break;

            case FADING:
                ballAlpha = EaseCubicOut(1.0f, 0.0f, timer, 3.0f);
                if (timer >= 3.0f)
                {
                    timer = 0.0f;
                    state = END;
                }
                break;

            case END:
                if (IsKeyPressed(KEY_ENTER))
                {
                    timer = 0.0f;
                    state = MOVE;

                    ballPositionX   = -SCREEN_WIDTH * 0.5f;
                    ballRadius      = 20;
                    ballAlpha       = 1.0f;
                }
                break;
            }

            // Render
            switch (state)
            {
            case END:
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GREEN);
                DrawText("PRESS [ENTER] TO PLAY AGAIN!", 240, 200, 20, BLACK);
                break;

            case FADING:
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GREEN);
                DrawCircle(ballPositionX, SCREEN_HEIGHT * 0.5f, ballRadius, Fade(RED, ballAlpha));
                break;

            default:
                DrawCircle(ballPositionX, SCREEN_HEIGHT * 0.5f, ballRadius, Fade(RED, ballAlpha));
                break;
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}