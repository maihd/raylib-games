#include <Mailib.h>
#include <GUI.h>

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    vec2 center = { .x = (screenWidth - 300) * 0.5f, .y = screenHeight * 0.5f };

    float innerRadius = 80.0f;
    float outerRadius = 190.0f;

    float startAngle = 0.0f;
    float endAngle = 360.0f;
    float segments = 0.0f;

    bool drawRing = true;
    bool drawRingLines = false;
    bool drawCircleLines = false;

    InitWindow(screenWidth, screenHeight, "Draw Ring");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawLine(500, 0, 500, GetScreenHeight(), Fade(LIGHTGRAY, 0.6f));
            DrawRectangle(500, 0, GetScreenWidth() - 500, GetScreenHeight(), Fade(LIGHTGRAY, 0.3f));

            if (drawRing) DrawRing(center, innerRadius, outerRadius, startAngle, endAngle, segments, Fade(MAROON, 0.3));
            if (drawRingLines) DrawRingLines(center, innerRadius, outerRadius, startAngle, endAngle, segments, Fade(BLACK, 0.4));
            if (drawCircleLines) DrawCircleSectorLines(center, outerRadius, startAngle, endAngle, segments, Fade(BLACK, 0.4));

            startAngle = GuiSlider((rect) { 600, 40, 120, 20 }, "Start Angle", TextFormat("%2.1f", startAngle), startAngle, -450, 450);
            endAngle = GuiSlider((rect) { 600, 70, 120, 20 }, "End Angle", TextFormat("%2.1f", endAngle), endAngle, -450, 450);

            innerRadius = GuiSliderBar((rect) { 600, 140, 120, 20 }, "Inner Radius", TextFormat("%2.1f", innerRadius), innerRadius, 0, 100, true);
            outerRadius = GuiSliderBar((rect) { 600, 170, 120, 20 }, "Outer Radius", TextFormat("%2.1f", outerRadius), outerRadius, 0, 200, true);

            segments = GuiSlider((rect) { .x = 600, .y = 240, .width = 120, .height = 20 }, "Segments", TextFormat("%2.0f", segments), segments, 0, 100);

            drawRing = GuiCheckBox((rect) { 600, 320, 20, 20 }, "Draw Ring", drawRing);
            drawRingLines = GuiCheckBox((rect) { 600, 350, 20, 20 }, "Draw Ring", drawRingLines);
            drawCircleLines = GuiCheckBox((rect) { 600, 380, 20, 20 }, "Draw Ring", drawCircleLines);

            DrawText(TextFormat("MODE: %s", (segments >= 4) ? "MANUAL" : "AUTO"), 600, 270, 10, (segments >= 4) ? MAROON : DARKGRAY);

            DrawFPS(10, 10);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}