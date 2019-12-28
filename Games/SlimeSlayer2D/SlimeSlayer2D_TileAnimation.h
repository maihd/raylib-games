#pragma once

#include <MaiDef.h>
#include "SlimeSlayer2D_TileSheet.h"

typedef struct TileAnimation
{
    TileSheet       sheet;
    Array(ivec2)    frames;

    float           timer;
    float           duration;

    bool            loop;
    bool            running;
} TileAnimation;

TileAnimation LoadTileAnimation(TileSheet sheet, float duration, bool loop, bool running, int frameCount, ...);
TileAnimation LoadTileAnimationWithFrames(TileSheet sheet, float duration, bool loop, bool running, Array(ivec2) frames);
void          UnloadTileAnimation(TileAnimation animation);

void UpdateTileAnimation(TileAnimation* animation, float timeStep);
void DrawTileAnimation(TileAnimation animation, vec2 position, vec2 pivot, bool flippedX, Color tint);