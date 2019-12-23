#include <raylib.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int MAX_FRAME_SPEED = 15;
    const int MIN_FRAME_SPEED = 1;

    InitWindow(screenWidth, screenHeight, "Textures");

    Texture scarfy = LoadTexture("../Assets/scarfy.png");

    Vector2 position = { 350.0f, 280.0f };
    Rectangle frameRec = { 0.0f, 0.0f, (float)scarfy.width / 6.0f, (float)scarfy.height };
    int currentFrame = 0;

    int frameCounter = 0;
    int frameInterval = 8;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        frameCounter++;

        if (frameCounter >= frameInterval)
        {
            frameCounter -= frameInterval;
            currentFrame++;

            if (currentFrame > 5)
            {
                currentFrame = 0;
            }

            frameRec.x = currentFrame * scarfy.width / 6.0f;
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            frameInterval--;
        }
        else if (IsKeyPressed(KEY_RIGHT))
        {
            frameInterval++;
        }

        if (frameInterval >= MAX_FRAME_SPEED)
        {
            frameInterval = MAX_FRAME_SPEED;
        }
        else if (frameInterval <= MIN_FRAME_SPEED)
        {
            frameInterval = MIN_FRAME_SPEED;
        }

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTexture(scarfy, 15, 40, WHITE);
            DrawRectangleLines(15, 40, scarfy.width, scarfy.height, LIME);
            DrawRectangleLines(15 + frameRec.x, 40 + frameRec.y, frameRec.width, frameRec.height, RED);

            DrawText("FRAME SPEED: ", 165, 210, 10, DARKGRAY);
            DrawText(FormatText("%02i FPS", frameInterval), 575, 210, 10, DARKGRAY);
            DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10, DARKGRAY);

            for (int i = 0; i < MAX_FRAME_SPEED; i++)
            {
                if (frameInterval > i)
                {
                    DrawRectangle(250 + 21 * i, 205, 20, 20, RED);
                }

                DrawRectangleLines(250 + 21 * i, 205, 20, 20, MAROON);
            }

            DrawTextureRec(scarfy, frameRec, position, WHITE);  // Draw part of the texture

            DrawText("(c) Scarfy sprite by Eiden Marsal", screenWidth - 200, screenHeight - 20, 10, GRAY);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}