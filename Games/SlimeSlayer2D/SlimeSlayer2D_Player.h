#pragma once

#include <MaiDef.h>
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
    float moveDirection;
    float hurtDirection;

    bool  attack;
    bool  hurt;
} PlayerInput;

typedef struct PlayerOutput
{
    bool hitboxingAttack;
} PlayerOutput;

typedef struct Player
{
    int             state;
    bool            flippedX;

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
} Player;

Player          PlayerNew(vec2 position, float moveSpeed, Color tint, 
    TileAnimation idleAnimation, 
    TileAnimation walkAnimation, 
    TileAnimation hurtAnimation, 
    TileAnimation attackAnimation, 
    TileAnimation dieAnimation);

void            PlayerFree(Player player);

PlayerOutput    PlayerUpdate(Player* player, PlayerInput input, float timeStep);
void            PlayerRender(Player player);
