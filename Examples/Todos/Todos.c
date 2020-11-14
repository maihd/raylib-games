#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#include <RayGui.h>

typedef struct Todo
{
    bool complete;
    char description[256];
} Todo;

bool GuiTextEditor(Rectangle bounds, char *text, int textLen, bool editMode);

int main(void)
{
    InitWindow(400, 600, "Todos");

    bool inputEditting = false;
    char inputDescription[256] = "";

    while (!WindowShouldClose())
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            GuiTextBox((Rectangle) { 10, 10, 380, 40 }, inputDescription, sizeof(inputDescription), false);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}