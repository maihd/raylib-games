#include <MaiLib.h>

int main(void)
{
    const int SCREEN_WIDTH  = 800;
    const int SCREEN_HEIGHT = 460;
    const float BALL_SPEED = 120.0f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RayGames - Basics");
    SetTargetFPS(60);

    vec2 ballPosition = { .x = SCREEN_WIDTH * 0.5f, .y = SCREEN_HEIGHT * 0.5f };

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT))
        {
            ballPosition.x += BALL_SPEED * deltaTime;
        }
        if (IsKeyDown(KEY_LEFT))
        {
            ballPosition.x -= BALL_SPEED * deltaTime;
        }

        if (IsKeyDown(KEY_UP))
        {
            ballPosition.y -= BALL_SPEED * deltaTime;
        }
        if (IsKeyDown(KEY_DOWN))
        {
            ballPosition.y += BALL_SPEED * deltaTime;
        }

        ballPosition.y -= GetMouseWheelMove() * BALL_SPEED * deltaTime;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            ballPosition = GetMousePosition();
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawCircleV(ballPosition, 50, MAROON);
            
            DrawText("Move the ball with arrow keys", 5, 5, 20, DARKGRAY);
            DrawText("Move the ball with holding left mouse and movement", 5, 30, 20, DARKGRAY);

            DrawFPS(5, SCREEN_HEIGHT - 20);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}