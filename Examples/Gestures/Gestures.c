#include <raylib.h>
#include <Array.h>

int main(void)
{
    const int SCREEN_WIDTH  = 800;
    const int SCREEN_HEIGHT = 450;

    enum {
        MAX_GESTURES = 21,
    };

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Gestures");

    Vector2 touchPosition = { 0, 0 };
    Rectangle touchArea = { 220, 10, SCREEN_WIDTH - 230, SCREEN_HEIGHT - 10 };

    int gesturesCount = 0;
    char gestures[MAX_GESTURES][64];

    int currentGesture = GESTURE_NONE;
    int lastGesture = GESTURE_NONE;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        lastGesture = currentGesture;
        currentGesture = GetGestureDetected();
        touchPosition = GetTouchPosition(0);

        if (CheckCollisionPointRec(touchPosition, touchArea) && (currentGesture != GESTURE_NONE))
        {
            if (currentGesture != lastGesture)
            {
                if (gesturesCount >= MAX_GESTURES - 1)
                {
                    gesturesCount = 0;
                }
                
                switch (currentGesture)
                {
                case GESTURE_TAP:           strcpy(gestures[gesturesCount], "GESTURE TAP"); break;
                case GESTURE_DOUBLETAP:     strcpy(gestures[gesturesCount], "GESTURE DOUBLETAP"); break;
                case GESTURE_HOLD:          strcpy(gestures[gesturesCount], "GESTURE HOLD"); break;
                case GESTURE_DRAG:          strcpy(gestures[gesturesCount], "GESTURE DRAG"); break;
                case GESTURE_SWIPE_RIGHT:   strcpy(gestures[gesturesCount], "GESTURE SWIPE RIGHT"); break;
                case GESTURE_SWIPE_LEFT:    strcpy(gestures[gesturesCount], "GESTURE SWIPE LEFT"); break;
                case GESTURE_SWIPE_DOWN:    strcpy(gestures[gesturesCount], "GESTURE SWIPE DOWN"); break;
                case GESTURE_SWIPE_UP:      strcpy(gestures[gesturesCount], "GESTURE SWIPE UP"); break;
                case GESTURE_PINCH_IN:      strcpy(gestures[gesturesCount], "GESTURE PINCH IN"); break;
                case GESTURE_PINCH_OUT:     strcpy(gestures[gesturesCount], "GESTURE PINCH OUT"); break;

                default: break;
                }

                gesturesCount++;
            }
        }

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawRectangleRec(touchArea, GRAY);
            DrawRectangle(225, 15, SCREEN_WIDTH - 240, SCREEN_HEIGHT - 30, RAYWHITE);

            DrawText("GESTURES TEST AREA", SCREEN_WIDTH - 270, SCREEN_HEIGHT - 40, 20, Fade(GRAY, 0.5f));

            for (int i = 0; i < gesturesCount; i++)
            {
                if (i % 2 == 0)
                {
                    DrawRectangle(10, 30 + 20 * i, 200, 20, Fade(LIGHTGRAY, 0.5f));
                }
                else
                {
                    DrawRectangle(10, 30 + 20 * i, 200, 20, Fade(LIGHTGRAY, 0.3f));
                }

                if (i < gesturesCount - 1)
                {
                    DrawText(gestures[i], 35, 36 + 20 * i, 10, DARKGRAY);
                }
                else
                {
                    DrawText(gestures[i], 35, 36 + 20 * i, 10, MAROON);
                }
            }

            DrawRectangleLines(10, 29, 200, SCREEN_HEIGHT - 50, GRAY);
            DrawText("DETECTED GESTURES", 50, 15, 10, GRAY);

            if (currentGesture != GESTURE_NONE)
            {
                DrawCircleV(touchPosition, 30, MAROON);
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}