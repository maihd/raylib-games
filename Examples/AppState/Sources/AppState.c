#include <raylib.h>

typedef struct
{
    int score;
    int highScore;
    int frames;
} AppState;

enum
{
    STORAGE_SCORE,
    STORAGE_HIGHSCORE,
};

int main(void)
{
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 450;

    Vector2 vector;
    vector = (Vector2){ 0, 0 };
    vector = (Vector2){ 1, 2 };

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "AppState");

    SetTargetFPS(60);

    AppState state = { 0 };

    while (!WindowShouldClose())
    {
        state.frames++;

        if (IsKeyPressed(KEY_R))
        {
            state.score = GetRandomValue(1000, 2000);
            state.highScore = GetRandomValue(3000, 4000);
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            StorageSaveValue(STORAGE_SCORE, state.score);
            StorageSaveValue(STORAGE_HIGHSCORE, state.highScore);
        }
        else if (IsKeyPressed(KEY_SPACE))
        {
            state.score = StorageLoadValue(STORAGE_SCORE);
            state.highScore = StorageLoadValue(STORAGE_HIGHSCORE);
        }

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawText(TextFormat("SCORE: %i", state.score), 280, 130, 40, MAROON);
            DrawText(TextFormat("HIGH-SCORE: %i", state.highScore), 210, 200, 40, BLACK);

            DrawText(TextFormat("frames: %i", state.frames), 10, 10, 20, LIME);

            DrawText("Press R to generate random numbers", 220, 40, 20, LIGHTGRAY);
            DrawText("Press ENTER to SAVE values", 250, 310, 20, LIGHTGRAY);
            DrawText("Press SPACE to LOAD values", 252, 350, 20, LIGHTGRAY);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}