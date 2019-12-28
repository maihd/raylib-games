#include <MaiLib.h>
#include <MaiMath.h>
#include <MaiArray.h>

#include "SlimeSlayer2D_Assets.h"
#include "SlimeSlayer2D_TileAnimation.h"

#ifdef RELEASE
#   define main __stdcall WinMain
#endif

int main(void)
{
    int tileWidth = 8;
    int tileHeight = 8;
    int screenWidth = tileWidth * 80;
    int screenHeight = tileHeight * 48;
    InitWindow(screenWidth, screenHeight, "Slime Slayer 2D");

    TileSheet playerSheet = LoadTileSheet(GetAssetsPath("Sprites/Player.png"), 7, 11);
    TileAnimation idleAnimation = LoadTileAnimation(playerSheet, 1.0f, true, true, 4,
        (ivec2) { 0, 0 },
        (ivec2) { 1, 0 },
        (ivec2) { 2, 0 },
        (ivec2) { 3, 0 }
    );
    TileAnimation walkAnimation = LoadTileAnimation(playerSheet, 1.0f, true, true, 6,
        (ivec2) { 1, 1 },
        (ivec2) { 2, 1 },
        (ivec2) { 3, 1 },
        (ivec2) { 4, 1 },
        (ivec2) { 5, 1 },
        (ivec2) { 6, 1 }
    );

    TileSheet backgroundSheet = LoadTileSheetWithTileSize(GetAssetsPath("Sprites/Background.png"), tileWidth, tileHeight);

    SetTargetFPS(60);

    float posX = screenWidth * 0.5f;
    float velX = 0.0f;

    bool flippedX = false;

    while (!PollWindowEvents())
    {
        if (IsKeyDown(KEY_LEFT))
        {
            velX = -tileWidth * 10.0f;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            velX = tileWidth * 10.0f;
        }
        else
        {
            velX = 0;
        }

        if (fabsf(velX) > 0.0f)
        {
            flippedX = velX < 0.0f;
        }

        posX = posX + velX * GetDeltaTime();

        if (fabsf(velX) > 0)
        {
            UpdateTileAnimation(&walkAnimation, GetDeltaTime());
        }
        else
        {
            UpdateTileAnimation(&idleAnimation, GetDeltaTime());
        }

        BeginDrawing();
        {
            ClearBackground(BLACK);
            
            if (fabsf(velX) > 0)
            {
                DrawTileAnimation(walkAnimation, vec2New(posX, screenHeight - 4 * tileWidth), vec2New(0.5f, 1.0f), flippedX, WHITE);
            }
            else
            {
                DrawTileAnimation(idleAnimation, vec2New(posX, screenHeight - 4 * tileWidth), vec2New(0.5f, 1.0f), flippedX, WHITE);
            }

            for (int x = 0; x <= screenWidth; x += tileWidth)
            {
                DrawTile(backgroundSheet, 2, 1, vec2New(x, screenHeight - 3 * tileWidth), vec2New(0, 1.0f), false, WHITE);
                DrawTile(backgroundSheet, 2, 2, vec2New(x, screenHeight - 2 * tileWidth), vec2New(0, 1.0f), false, WHITE);
                DrawTile(backgroundSheet, 2, 2, vec2New(x, screenHeight - 1 * tileWidth), vec2New(0, 1.0f), false, WHITE);
                DrawTile(backgroundSheet, 2, 2, vec2New(x, screenHeight - 0 * tileWidth), vec2New(0, 1.0f), false, WHITE);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
