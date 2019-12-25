#include <MaiLib.h>

static int _frameCount = 0;

bool PollWindowEvents(void)
{
    _frameCount++;
    return WindowShouldClose();
}

int GetFrameCount(void)
{
    return _frameCount;
}

float GetDeltaTime(void)
{
    return GetFrameTime();
}

float GetTotalTime(void)
{
    return GetTime();
}

Color ColorFromVec4(vec4 color)
{
    const float factor = 255;
    Color result = (Color){
        .r = (byte)(color.x * factor),
        .g = (byte)(color.y * factor),
        .b = (byte)(color.z * factor),
        .a = (byte)(color.w * factor),
    };

    return result;
}

