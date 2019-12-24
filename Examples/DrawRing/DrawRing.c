#include <Mailib.h>
#include <GUI.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;

    float startAngle = 0.0f;
    float endAngle = 0.0f;

    InitWindow(screenWidth, screenHeight, "Draw Ring");

    vec2 pos = { 0, 0 };

    while (!WindowShouldClose())
    {
        

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            startAngle = GuiSliderBar((rect){ 600, 40, 120, 20 }, "Start Angle", TextFormat("%2.1f", startAngle), startAngle, -450, 450);

            DrawRectangle(pos.x, pos.y, screenWidth, screenHeight, BLACK);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}