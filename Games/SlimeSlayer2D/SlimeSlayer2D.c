#include <MaiLib.h>
#include <MaiMath.h>
#include <MaiArray.h>

typedef struct TileSheet
{
    int cols;
    int rows;

    Texture texture;
} TileSheet;

typedef struct TileAnimation
{
    TileSheet       sheet;
    Array(ivec2)    frames;

    float           timer;
    float           duration;

    bool            loop;
    bool            running;
} TileAnimation;

typedef enum PlayerState
{
    PLAYER_NONE,
    PLAYER_IDLE,
    PLAYER_WALK,
    PLAYER_ATTACK,
    PLAYER_HURT,
    PLAYER_DIE,
} PlayerState;

typedef struct PlayerInput
{
    float direction;
    bool  attack;
} PlayerInput;

typedef struct Player
{
    int             state;

    float           moveSpeed;
    vec2            velocity;

    vec2            position;
    vec2            scale;

    Color           tint;
    TileAnimation   animation;
} Player;

TileSheet LoadTileSheet(const char* path, int cols, int rows);
TileSheet LoadTileSheetWithTileSize(const char* path, int tileWidth, int tileHeight);
void      UnloadTileSheet(TileSheet sheet);

void      DrawTile(TileSheet sheet, int col, int row, vec2 position, vec2 pivot, bool flippedX, Color tint);

TileAnimation LoadTileAnimation(TileSheet sheet, float duration, bool loop, bool running, int frameCount, ...);
TileAnimation LoadTileAnimationWithFrames(TileSheet sheet, float duration, bool loop, bool running, Array(ivec2) frames);
void          UnloadTileAnimaiton(TileAnimation animation);

void UpdateTileAnimation(TileAnimation* animation, float timeStep);
void DrawTileAnimation(TileAnimation animation, vec2 position, vec2 pivot, bool flippedX, Color tint);

const char* GetAssetsPath(const char* targetPath);

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

TileSheet LoadTileSheet(const char* path, int cols, int rows)
{
    Texture texture = LoadTexture(path);
    if (texture.id)
    {
        return (TileSheet){
            .cols = cols,
            .rows = rows,
            .texture = texture
        };
    }
    else
    {
        return (TileSheet) { 0 };
    }
}

TileSheet LoadTileSheetWithTileSize(const char* path, int tileWidth, int tileHeight)
{
    Texture texture = LoadTexture(path);
    if (texture.id)
    {
        return (TileSheet) {
            .cols = texture.width / tileWidth,
            .rows = texture.height / tileHeight,
            .texture = texture
        };
    }
    else
    {
        return (TileSheet) { 0 };
    }
}

void UnloadTileSheet(TileSheet sheet)
{
    UnloadTexture(sheet.texture);
}

void DrawTile(TileSheet sheet, int col, int row, vec2 position, vec2 pivot, bool flippedX, Color tint)
{
    float tileWidth = 1.0f / sheet.cols * sheet.texture.width;
    float tileHeight = 1.0f / sheet.rows * sheet.texture.height;

    rect tileRect = rectNew(col * tileWidth, row * tileHeight, (flippedX ? -1.0f : 1.0f) * tileWidth, tileHeight);
    rect drawRect = rectNew(position.x, position.y, tileWidth, tileHeight);
    
    DrawTexturePro(sheet.texture, tileRect, drawRect, vec2New(pivot.x * tileWidth, pivot.y * tileHeight), 0.0f, tint);
}

TileAnimation LoadTileAnimation(TileSheet sheet, float duration, bool loop, bool running, int frameCount, ...)
{
    Array(ivec2) frames = ArrayNew(ivec2, frameCount);
    if (!frames)
    {
        return (TileAnimation) { 0 };
    }

    ArraySetCount(frames, frameCount);

    ArgList arglist;
    BeginArgList(arglist, frameCount);
    for (int i = 0; i < frameCount; i++)
    {
        ivec2 frame = GetArg(arglist, ivec2);
        frames[i] = frame;
    }
    EndArgList(arglist);

    return (TileAnimation) {
        .sheet = sheet,
        .frames = frames,

        .duration = duration,
        .loop = loop,
        .running = running,

        .timer = 0.0f,
    };
}

TileAnimation LoadTileAnimationWithFrames(TileSheet sheet, float duration, bool loop, bool running, Array(ivec2) frames)
{
    if (!frames)
    {
        return (TileAnimation) { 0 };
    }

    return (TileAnimation) {
        .sheet = sheet,
        .frames = frames,

        .duration = duration,
        .loop = loop,
        .running = running,

        .timer = 0.0f,
    };
}

void UnloadTileAnimaiton(TileAnimation animation)
{
    ArrayFree(animation.frames);
}

void UpdateTileAnimation(TileAnimation* animation, float timeStep)
{
    if (animation->running)
    {
        TileAnimation newAnimation = *animation;
        newAnimation.timer += timeStep;
        if (newAnimation.timer >= newAnimation.duration)
        {
            newAnimation.timer -= newAnimation.duration;
            newAnimation.running = newAnimation.loop;
        }

        *animation = newAnimation;
    }
}

void DrawTileAnimation(TileAnimation animation, vec2 position, vec2 pivot, bool flippedX, Color tint)
{
    int current = (int)(animation.timer / animation.duration * ArrayCount(animation.frames));
    ivec2 frame = animation.frames[current];

    DrawTile(animation.sheet, frame.x, frame.y, position, pivot, flippedX, tint);
}

const char* GetAssetsPath(const char* targetPath) 
{
#ifdef RELEASE
    const char* ASSET_PATH = "Assets";
#else
    const char* ASSET_PATH = "../Binary/SlimeSlayer2D/Assets";
#endif

    return TextFormat("%s/%s", ASSET_PATH, targetPath);
}
