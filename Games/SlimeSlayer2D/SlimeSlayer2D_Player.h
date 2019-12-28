#pragma once

#include "SlimeSlayer2D_TileAnimation.h"

typedef enum PlayerState
{
    PLAYER_NONE,
    PLAYER_IDLE,
    PLAYER_WALK,
    PLAYER_ATTACK,
    PLAYER_HURT,
    PLAYER_DIE,
} PlayerState;

typedef struct PlayerInput
{
    float direction;
    bool  attack;
} PlayerInput;

typedef struct Player
{
    int             state;

    float           moveSpeed;
    vec2            velocity;

    vec2            position;
    vec2            scale;

    Color           tint;
    TileAnimation   animation;
} Player;
