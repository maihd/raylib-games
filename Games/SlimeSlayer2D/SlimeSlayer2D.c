#include <assert.h>

#include <MaiLib.h>
#include <MaiMath.h>
#include <MaiArray.h>
#include <MaiFreeList.h>

#include "SlimeSlayer2D_World.h"
#include "SlimeSlayer2D_Slime.h"
#include "SlimeSlayer2D_Player.h"

#include "SlimeSlayer2D_Assets.h"
#include "SlimeSlayer2D_TileAnimation.h"

#ifdef RELEASE
#   define main __stdcall WinMain
#endif

World LoadWorld(int tileCols, int tileRows, int tileWidth, int tileHeight);
void  UnloadWorld(World world);

Slime LoadSlime(void);
void  UnloadSlime(Slime slime);

Player LoadPlayer(void);
void   UnloadPlayer(Player player);

int main(void)
{
    int tileCols = 80;
    int tileRows = 48;
    int tileWidth = 8;
    int tileHeight = 8;
    int screenWidth = tileWidth * tileCols;
    int screenHeight = tileHeight * tileRows;

    InitWindow(screenWidth, screenHeight, "Slime Slayer 2D");
    SetTargetFPS(60);

    Player playerPrefab = LoadPlayer();
    Slime slimePrefab = LoadSlime();
    World worldPrefab = LoadWorld(tileCols, tileRows, tileWidth, tileHeight);


    Array(Slime) slimes = ArrayNew(Slime, 128);

    World world = worldPrefab;

    Player player = playerPrefab;
    player.position = vec2New(screenWidth * 0.5f, screenHeight - tileHeight * world.groundHeight);

    float minSpawnInterval = 1.25f;
    float maxSpawnInterval = 2.35f;

    float spawnRate = 1.0f;

    float spawnInterval = maxSpawnInterval;
    float spawnTimer    = spawnInterval;

    enum GameState {
        GAME_STARTING,
        GAME_PLAYING,
        GAME_FINALIZING,
        GAME_OVER,
    } gameState = GAME_STARTING;
    float gameOverlay = 1.0f;

    Font font = LoadFont(GetAssetsPath("Fonts/m5x7.ttf"));

    int slayedSlimesCount = 0;

    while (!PollWindowEvents())
    {
        PlayerInput playerInput = { 0 };

        if (gameState == GAME_PLAYING)
        {
            if (IsKeyDown(KEY_LEFT))
            {
                playerInput.moveDirection = -1.0f;
            }
            else if (IsKeyDown(KEY_RIGHT))
            {
                playerInput.moveDirection = 1.0f;
            }
            else
            {
                playerInput.moveDirection = 0;
            }

            if (IsKeyPressed(KEY_SPACE))
            {
                playerInput.attack = true;
            }
        }

        WorldUpdate(&world, GetDeltaTime());

        PlayerOutput playerOutput = PlayerUpdate(&player, playerInput, GetDeltaTime());
        if (player.state == PLAYER_DIE && !player.animation.running)
        {
            gameState = GAME_FINALIZING;
        }

        bool slayedOnce = false;
        for (int i = 0, n = ArrayCount(slimes); i < n; i++)
        {
            if (player.state != PLAYER_HURT && player.state != PLAYER_DIE)
            {
                Slime slime = slimes[i];

                SlimeInput input = { 0 };
                input.moveDirection = player.position.x > slime.position.x ? 1.0f : -1.0f;

                bool orientationLeft = slime.originFlippedX - slime.flippedX;
                if ((orientationLeft && player.position.x < slime.position.x) || (!orientationLeft && player.position.x > slime.position.x))
                {
                    input.attack = fabsf(player.position.x - slime.position.x) < slime.attackRange;
                }

                if (!slayedOnce && playerOutput.hitboxingAttack)
                {
                    bool playerOrientationLeft = player.flippedX;
                    if ((playerOrientationLeft && player.position.x > slime.position.x) || (!playerOrientationLeft && player.position.x < slime.position.x))
                    {
                        input.hurt = fabsf(player.position.x - slime.position.x) < player.attackRange;
                        input.hurtDirection = playerOrientationLeft ? -0.1f : 0.1f;
                    }
                }

                SlimeOutput output = SlimeUpdate(&slime, input, GetDeltaTime());
                if (output.hitboxingAttack)
                {
                    bool orientationLeft = slime.originFlippedX - slime.flippedX;
                    if ((orientationLeft && player.position.x < slime.position.x) || (!orientationLeft && player.position.x > slime.position.x))
                    {
                        PlayerInput playerInput = { 0 };
                        playerInput.hurt = fabsf(player.position.x - slime.position.x) < slime.attackRange;
                        playerInput.hurtDirection = orientationLeft ? 0.5f : -0.5f;

                        if (playerInput.hurt)
                        {
                            PlayerUpdate(&player, playerInput, 0.0f);
                        }
                    }
                }

                if (output.removeSelf)
                {
                    slayedSlimesCount++;

                    ArrayEraseWithHole(slimes, i);
                    int arrayCount = ArrayCount(slimes);
                    if (arrayCount == n)
                    {
                        assert(0);
                    }
                    i--; n--;
                }
                else
                {
                    slimes[i] = slime;
                }
            }
            else
            {
                SlimeInput input = { 0 };
                SlimeOutput output = SlimeUpdate(&slimes[i], input, GetDeltaTime());

                if (output.removeSelf)
                {
                    slayedSlimesCount++;

                    ArrayEraseWithHole(slimes, i);
                    i--; n--;
                }
            }
        }

        switch (gameState)
        {
        case GAME_PLAYING:
            spawnTimer += GetDeltaTime();
            if (spawnTimer >= spawnInterval)
            {
                spawnTimer -= spawnInterval;

                bool shouldSpawn = GetRandomValue(30, (int)(spawnRate * 100)) <= 60;
                if (shouldSpawn)
                {
                    bool left = GetRandomValue(0, 1) == 0;

                    Slime slime = slimePrefab;
                    slime.position = vec2New(left ? -screenWidth * 0.25f : screenWidth * 1.25f, screenHeight - tileHeight * world.groundHeight);

                    ArrayPush(slimes, slime);

                    spawnRate = spawnRate * GetRandomValue(80, 100) * 0.01f;
                    spawnRate = clampf(spawnRate, 0.5f, 1.0f);

                    spawnInterval = spawnInterval * GetRandomValue(97, 100) * 0.01f;
                    spawnInterval = clampf(spawnInterval, minSpawnInterval, maxSpawnInterval);
                }
            }
            break;

        case GAME_STARTING:
            gameOverlay = gameOverlay - GetDeltaTime();
            if (gameOverlay <= 0.0f)
            {
                gameOverlay = 0.0f;

                gameState = GAME_PLAYING;
            }
            break;

        case GAME_FINALIZING:
            gameOverlay = gameOverlay + GetDeltaTime();
            if (gameOverlay >= 1.0f)
            {
                gameOverlay = 1.0f;

                spawnRate = 1.0f;

                spawnInterval = maxSpawnInterval;
                spawnTimer = spawnInterval;

                world = worldPrefab;
                player = playerPrefab;
                player.position = vec2New(screenWidth * 0.5f, screenHeight - tileHeight * world.groundHeight);

                ArrayClear(slimes);

                gameState = GAME_OVER;
            }
            break;

        case GAME_OVER:
            if (IsKeyPressed(KEY_SPACE))
            {
                gameState = GAME_STARTING;
                gameOverlay = 1.0f;
            }
            break;
        }
        

        BeginDrawing();
        {
            ClearBackground(BLACK);
            
            WorldRender(world);
            PlayerRender(player);

            for (int i = 0, n = ArrayCount(slimes); i < n; i++)
            {
                SlimeRender(slimes[i]);
            }

            DrawTextEx(font, TextFormat("Slayed Slimes: %d", slayedSlimesCount), vec2New(5, 5), 24.0f, 2.0f, WHITE);
            DrawTextEx(font, "- Left/Right arrow to move", vec2New(5, 35), 16.0f, 2.0f, WHITE); 
            DrawTextEx(font, "- SPACE to slash", vec2New(5, 50), 16.0f, 2.0f, WHITE);

            switch (gameState)
            {
            case GAME_STARTING:
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, gameOverlay));
                break;

            case GAME_FINALIZING:
            case GAME_OVER: {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, gameOverlay));

                const char* text = "GAME OVER";
                float fontSize = 48.0f;
                float fontSpace = 2.0f;
                vec2 textSize = MeasureTextEx(font, text, fontSize, fontSpace);

                DrawTextRec(font, text, rectNew(screenWidth * 0.5f - textSize.x * 0.5f, screenHeight * 0.5f - textSize.y * 0.5f, textSize.x, textSize.y), fontSize, fontSpace, false, WHITE);
            } break;
            }

            if (gameState == GAME_OVER)
            {
                const char* text = "PRESS SPACE TO RETRY";
                float fontSize = 24.0f;
                float fontSpace = 2.0f;
                vec2 textSize = MeasureTextEx(font, text, fontSize, fontSpace);

                DrawTextRec(font, text, rectNew(screenWidth * 0.5f - textSize.x * 0.5f, screenHeight * 0.7f - textSize.y * 0.5f, textSize.x, textSize.y), fontSize, fontSpace, false, WHITE);
            }
        }
        EndDrawing();
    }

    UnloadFont(font);

    UnloadPlayer(playerPrefab);
    UnloadSlime(slimePrefab);
    CloseWindow();
    return 0;
}

World LoadWorld(int tileCols, int tileRows, int tileWidth, int tileHeight)
{
    TileSheet worldSheet = LoadTileSheetWithTileSize(GetAssetsPath("Sprites/World.png"), tileWidth, tileHeight);
    World world = WorldNew(worldSheet, tileCols, tileRows, tileWidth, tileHeight, 4, (ivec2) { 18, 4 }, (ivec2) { 2, 1 }, (ivec2) { 2, 2 });
    return world;
}

void UnloadWorld(World world)
{
    UnloadTileSheet(world.tileSheet);
}

Slime LoadSlime(void) 
{
    TileSheet sheet = LoadTileSheet(GetAssetsPath("Sprites/Slime.png"), 8, 3);
    TileAnimation idleAnimation = LoadTileAnimation(sheet, 1.0f, true, true, 4,
        (ivec2) { 0, 0 },
        (ivec2) { 1, 0 },
        (ivec2) { 2, 0 },
        (ivec2) { 3, 0 }
    );
    TileAnimation walkAnimation = LoadTileAnimation(sheet, 1.0f, true, true, 4,
        (ivec2) { 4, 0 },
        (ivec2) { 5, 0 },
        (ivec2) { 6, 0 },
        (ivec2) { 7, 0 }
    );
    TileAnimation attackAnimation = LoadTileAnimation(sheet, 0.65f, false, true, 5,
        (ivec2) { 0, 1 },
        (ivec2) { 1, 1 },
        (ivec2) { 2, 1 },
        (ivec2) { 3, 1 },
        (ivec2) { 4, 1 }
    );
    TileAnimation hurtAnimation = LoadTileAnimation(sheet, 0.5f, false, true, 3,
        (ivec2) { 5, 1 },
        (ivec2) { 6, 1 },
        (ivec2) { 7, 1 }
    );
    TileAnimation dieAnimation = LoadTileAnimation(sheet, 0.5f, false, true, 5,
        (ivec2) { 0, 2 },
        (ivec2) { 1, 2 },
        (ivec2) { 2, 2 },
        (ivec2) { 3, 2 },
        (ivec2) { 4, 2 }
    );
    Slime slime = SlimeNew(vec2New(0, 0), true, 0, WHITE, 
        idleAnimation, walkAnimation, 
        hurtAnimation, attackAnimation, dieAnimation);
    slime.moveSpeed   = 80.0f;
    slime.attackDelay = 0.3f;
    slime.attackRange = 10.0f;

    return slime;
}

void UnloadSlime(Slime slime)
{
    UnloadTileSheet(slime.idleAnimation.sheet);

    UnloadTileAnimation(slime.idleAnimation);
    UnloadTileAnimation(slime.hurtAnimation);
    UnloadTileAnimation(slime.walkAnimation);
    UnloadTileAnimation(slime.attackAnimation);
    UnloadTileAnimation(slime.dieAnimation);
}

Player LoadPlayer(void)
{
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
    TileAnimation attackAnimation = LoadTileAnimation(playerSheet, 0.35f, false, true, 6,
        (ivec2) { 5, 6 },
        (ivec2) { 6, 6 },
        (ivec2) { 0, 7 },
        (ivec2) { 1, 7 },
        (ivec2) { 2, 7 },
        (ivec2) { 3, 7 }
    );
    TileAnimation hurtAnimation = LoadTileAnimation(playerSheet, 0.5f, false, true, 5,
        (ivec2) { 5, 4 },
        (ivec2) { 6, 4 },
        (ivec2) { 0, 5 },
        (ivec2) { 1, 5 },
        (ivec2) { 2, 5 }
    );
    TileAnimation dieAnimation = LoadTileAnimation(playerSheet, 0.5f, false, true, 6,
        (ivec2) { 0, 9 },
        (ivec2) { 1, 9 },
        (ivec2) { 2, 9 },
        (ivec2) { 3, 9 },
        (ivec2) { 4, 9 },
        (ivec2) { 5, 9 }
    );
    Player player = PlayerNew(vec2New(0, 0), 0, WHITE, 
        idleAnimation, walkAnimation, 
        hurtAnimation, attackAnimation, dieAnimation);
    player.moveSpeed   = 120.0f;
    player.attackDelay = 0.1f;
    player.attackRange = 30.0f;

    return player;
}

void UnloadPlayer(Player player)
{
    UnloadTileSheet(player.idleAnimation.sheet);

    UnloadTileAnimation(player.idleAnimation);
    UnloadTileAnimation(player.hurtAnimation);
    UnloadTileAnimation(player.walkAnimation);
    UnloadTileAnimation(player.attackAnimation);
    UnloadTileAnimation(player.dieAnimation);
}
