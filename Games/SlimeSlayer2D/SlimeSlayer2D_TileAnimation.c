#include <MaiArray.h>

#include "SlimeSlayer2D_TileAnimation.h"

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