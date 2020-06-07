#include <stdio.h>
#include <raylib.h>

typedef enum
{
    UI_BUTTON_NORMAL,
    UI_BUTTON_PRESSED,
    UI_BUTTON_PRESSING, 
    UI_BUTTON_RELEASED,
} UiButtonState;

typedef struct UiButton
{
    int state;
    
    Rectangle bounds;
    const char* text;
} UiButton;

void UpdateButton(UiButton* button);
void DrawButton(UiButton button);

int main(void)
{
    InitWindow(800, 600, "UI examples");
    SetTargetFPS(60);

    UiButton button = {
        UI_BUTTON_NORMAL,
        
        { 100, 100, 200, 40 },
        "Click me!"
    };

    while (!WindowShouldClose())
    {
        UpdateButton(&button);
        if (button.state == UI_BUTTON_PRESSED)
        {
            printf("Clicked!\n");
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawButton(button);

        EndDrawing();    
    }

    CloseWindow();
    return 0;
}

void UpdateButton(UiButton* button)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), button->bounds))
    {
        button->state = UI_BUTTON_PRESSED;
    }
    else if (button->state == UI_BUTTON_PRESSED)
    {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            button->state = UI_BUTTON_RELEASED;
        }
        else
        {
            button->state = UI_BUTTON_PRESSING;
        }
    }
    else if (button->state == UI_BUTTON_RELEASED)
    {
        button->state = UI_BUTTON_NORMAL;
    }
}

void DrawButton(UiButton button)
{
    Color buttonColor = button.state == UI_BUTTON_PRESSED ? DARKGRAY : GRAY;
    DrawRectangleRec(button.bounds, buttonColor);

    float centerX = button.bounds.x + button.bounds.width * 0.5f;
    float centerY = button.bounds.y + button.bounds.height * 0.5f;
    float textWidth = MeasureText(button.text, 24);
    float textX = centerX - textWidth * 0.5f;
    float textY = centerY - 12;
    DrawText(button.text, textX, textY, 24, BLACK);
}