#include <raylib.h>

#define MAX_COLORS_COUNT    21          // Number of colors available

int main(void)
{
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shapes");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

#if 0
            DrawText("Some basics shapes available on raylib", 20, 20, 20, DARKGRAY);

            DrawCircle(SCREEN_WIDTH * 0.25f, 120, 25, DARKBLUE);
            
            DrawCircleGradient(SCREEN_WIDTH * 0.25f, 220, 60, GREEN, SKYBLUE);

            DrawRectangle(SCREEN_WIDTH * 0.5f - 60, 100, 120, 60, RED);
            DrawRectangleLines(SCREEN_WIDTH * 0.5f - 40, 320, 80, 60, ORANGE);
            DrawRectangleGradientH(SCREEN_WIDTH * 0.5f - 90, 170, 180, 130, MAROON, GOLD);

            DrawTriangle(
                (Vector2) {SCREEN_WIDTH * 0.75f, 80},
                (Vector2) {SCREEN_WIDTH * 0.75f - 60, 150},
                (Vector2) {SCREEN_WIDTH * 0.75f + 60, 150},
                VIOLET
                );

            DrawPoly((Vector2) { SCREEN_WIDTH * 0.75f, 320.0f }, 8, 80, 0, BROWN);

            DrawLine(18, 42, SCREEN_WIDTH - 18, 42, BLACK);
            DrawCircleLines(SCREEN_WIDTH * 0.25f, 340, 80, DARKBLUE);
            DrawTriangleLines(
                (Vector2) {SCREEN_WIDTH * 0.75f, 160},
                (Vector2) {SCREEN_WIDTH * 0.75f - 20, 230},
                (Vector2) {SCREEN_WIDTH * 0.75f + 20, 230},
                DARKBLUE
            );
#endif

            const int ROWS = 3;
            const int COLS = 7;
            const int PADDING = 5;
            const int CELL_SIZE = (SCREEN_WIDTH - (COLS - 1) * PADDING) / COLS;

            float startX = PADDING * 0.5f;
            float startY = PADDING * 0.5f + SCREEN_HEIGHT * 0.5f - (ROWS * CELL_SIZE + (ROWS - 1) * PADDING) * 0.5f;

            Color colors[MAX_COLORS_COUNT] = {
                DARKGRAY, MAROON, ORANGE, DARKGREEN, DARKBLUE, DARKPURPLE, DARKBROWN,
                GRAY, RED, GOLD, LIME, BLUE, VIOLET, BROWN, LIGHTGRAY, PINK, YELLOW,
                GREEN, SKYBLUE, PURPLE, BEIGE 
            };

            const char *colorNames[MAX_COLORS_COUNT] = {
                "DARKGRAY", "MAROON", "ORANGE", "DARKGREEN", "DARKBLUE", "DARKPURPLE",
                "DARKBROWN", "GRAY", "RED", "GOLD", "LIME", "BLUE", "VIOLET", "BROWN",
                "LIGHTGRAY", "PINK", "YELLOW", "GREEN", "SKYBLUE", "PURPLE", "BEIGE" 
            };

            for (int i = 0; i < ROWS; i++)
            {
                for (int j = 0; j < COLS; j++)
                {
                    const Color color = colors[i * COLS + j];
                    const char* colorName = colorNames[i * COLS + j];

                    float x = startX + j * (CELL_SIZE + PADDING);
                    float y = startY + i * (CELL_SIZE + PADDING);
                    Rectangle rect = { x, y, CELL_SIZE, CELL_SIZE };
                    
                    DrawRectangleRec(rect, color);

                    if (IsKeyDown(KEY_SPACE) || CheckCollisionPointRec(GetMousePosition(), rect))
                    {
                        DrawRectangleLinesEx(rect, 12, Fade(BLACK, 0.3f));
                        DrawRectangle(rect.x, rect.y + rect.height - 52, rect.width, 40, Fade(BLACK, 0.8f));

                        DrawText(colorName, rect.x + rect.width - MeasureText(colorName, 20) - 20,
                            rect.y + rect.height - 40, 20, color);
                    }
                }
            }

            DrawText("Hold SPACE to see all color", 5, 5, 20, GRAY);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}