#pragma once

#include <MaiDef.h>
#include "SlimeSlayer2D_TileAnimation.h"

typedef enum SlimeState
{
    SLIME_NONE,
    SLIME_IDLE,
    SLIME_WALK,
    SLIME_ATTACK,
    SLIME_HURT,
    SLIME_DIE,
} SlimeState;

typedef struct SlimeInput
{
    float moveDirection;
    float hurtDirection;

    bool  attack;
    bool  hurt;
} SlimeInput;

typedef struct SlimeOutput
{
    bool hitboxingAttack;
    bool removeSelf;
} SlimeOutput;

typedef struct Slime
{
    int             state;
    bool            flippedX;
    bool            originFlippedX;

    float           moveSpeed;
    float           direction;

    float           attackRange;
    float           attackDelay;

    vec2            position;
    vec2            scale;

    Color           tint;
    TileAnimation   animation;

    TileAnimation   idleAnimation;
    TileAnimation   walkAnimation;
    TileAnimation   hurtAnimation;
    TileAnimation   attackAnimation;
    TileAnimation   dieAnimation;
} Slime;

Slime           SlimeNew(vec2 position, bool originFlippedX, float moveSpeed, Color tint,
    TileAnimation idleAnimation,
    TileAnimation walkAnimation,
    TileAnimation hurtAnimation,
    TileAnimation attackAnimation,
    TileAnimation dieAnimation);

void            SlimeFree(Slime slime);

SlimeOutput     SlimeUpdate(Slime* slime, SlimeInput input, float timeStep);
void            SlimeRender(Slime slime);
