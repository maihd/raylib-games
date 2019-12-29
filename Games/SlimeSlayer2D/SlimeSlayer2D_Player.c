#include <MaiLib.h>
#include <MaiMath.h>

#include "SlimeSlayer2D_Player.h"

Player PlayerNew(vec2 position, float moveSpeed, Color tint,
    TileAnimation idleAnimation,
    TileAnimation walkAnimation,
    TileAnimation hurtAnimation,
    TileAnimation attackAnimation,
    TileAnimation dieAnimation)
{
    return (Player){
        .state = PLAYER_IDLE,
        
        .flippedX = false,

        .moveSpeed = moveSpeed,
        .direction = 0.0f,

        .position = position,
        .tint = tint,

        .animation = idleAnimation,

        .idleAnimation = idleAnimation,
        .walkAnimation = walkAnimation,
        .hurtAnimation = hurtAnimation,
        .attackAnimation = attackAnimation,
        .dieAnimation = dieAnimation
    };
}

void PlayerFree(Player player)
{
    UnloadTileAnimation(player.idleAnimation);
    UnloadTileAnimation(player.walkAnimation);
    UnloadTileAnimation(player.hurtAnimation);
    UnloadTileAnimation(player.attackAnimation);
    UnloadTileAnimation(player.dieAnimation);
}

PlayerOutput PlayerUpdate(Player* player, PlayerInput input, float timeStep)
{
    PlayerOutput output = { 0 };

    Player newPlayer = *player;
    UpdateTileAnimation(&newPlayer.animation, timeStep);

    switch (newPlayer.state)
    {
    case PLAYER_IDLE:
        if (input.hurt)
        {
            newPlayer.state = PLAYER_HURT;
            newPlayer.direction = input.hurtDirection;
            newPlayer.animation = newPlayer.hurtAnimation;
        }
        else if (input.attack)
        {
            newPlayer.state = PLAYER_ATTACK;
            newPlayer.direction = 0.0f;
            newPlayer.animation = newPlayer.attackAnimation;
        }
        if (fabsf(input.moveDirection) > 0.0f)
        {
            newPlayer.state     = PLAYER_WALK;
            newPlayer.direction = input.moveDirection;
            newPlayer.flippedX  = input.moveDirection < 0.0f;
            newPlayer.animation = newPlayer.walkAnimation;
        }
        break;

    case PLAYER_WALK:
        if (input.hurt)
        {
            newPlayer.state = PLAYER_HURT;
            newPlayer.direction = input.hurtDirection;
            newPlayer.animation = newPlayer.hurtAnimation;
        }
        else if (input.attack)
        {
            newPlayer.state     = PLAYER_ATTACK;
            newPlayer.direction = 0.0f;
            newPlayer.animation = newPlayer.attackAnimation;
        }
        else if (fabsf(input.moveDirection) == 0.0f)
        {
            newPlayer.state     = PLAYER_IDLE;
            newPlayer.direction = input.moveDirection;
            newPlayer.animation = newPlayer.idleAnimation;
        }
        else
        {
            newPlayer.direction = input.moveDirection;
            newPlayer.flippedX  = input.moveDirection < 0.0f;
            newPlayer.position  = vec2Add(newPlayer.position, vec2New(newPlayer.direction * newPlayer.moveSpeed * timeStep, 0.0f));
        }
        break;

    case PLAYER_ATTACK:
        if (input.hurt)
        {
            newPlayer.state     = PLAYER_HURT;
            newPlayer.direction = input.hurtDirection;
            newPlayer.animation = newPlayer.hurtAnimation;
        }
        else if (!newPlayer.animation.running)
        {
            if (input.attack)
            {
                newPlayer.state = PLAYER_ATTACK;
                newPlayer.direction = 0.0f;
                newPlayer.animation = newPlayer.attackAnimation;
            }
            else if (fabsf(input.moveDirection) > 0.0f)
            {
                newPlayer.state = PLAYER_WALK;
                newPlayer.direction = input.moveDirection;
                newPlayer.flippedX  = input.moveDirection < 0.0f;
                newPlayer.animation = newPlayer.walkAnimation;
            }
            else
            {
                newPlayer.state = PLAYER_IDLE;
                newPlayer.direction = input.moveDirection;
                newPlayer.animation = newPlayer.idleAnimation;
            }
        }
        else
        {
            if (newPlayer.animation.timer >= newPlayer.attackDelay)
            {
                output.hitboxingAttack = true;
            }
        }
        break;

    case PLAYER_HURT:
        if (!newPlayer.animation.running)
        {
            //if (input.attack)
            //{
            //    newPlayer.state     = PLAYER_ATTACK;
            //    newPlayer.direction = 0.0f;
            //    newPlayer.animation = newPlayer.attackAnimation;
            //}
            //else if (fabsf(input.moveDirection) > 0.0f)
            //{
            //    newPlayer.state     = PLAYER_WALK;
            //    newPlayer.direction = input.moveDirection;
            //    newPlayer.flippedX  = input.moveDirection < 0.0f;
            //    newPlayer.animation = newPlayer.walkAnimation;
            //}
            //else
            //{
            //    newPlayer.state     = PLAYER_IDLE;
            //    newPlayer.direction = input.moveDirection;
            //    newPlayer.animation = newPlayer.idleAnimation;
            //}
            newPlayer.state     = PLAYER_DIE;
            newPlayer.animation = newPlayer.dieAnimation;
        }
        else
        {
            newPlayer.flippedX = newPlayer.direction > 0.0f;
            newPlayer.position = vec2Add(newPlayer.position, vec2New(newPlayer.direction * newPlayer.moveSpeed * timeStep, 0.0f));
        }
        break;

    case PLAYER_DIE:
        // Ignore all input
        if (newPlayer.animation.running)
        {
            newPlayer.tint = Fade(WHITE, fmaxf(0.0f, 1.0f - newPlayer.animation.timer / newPlayer.animation.duration));
        }
        break;
    }

    *player = newPlayer;

    return output;
}

void PlayerRender(Player player)
{
    DrawTileAnimation(player.animation, player.position, vec2New(0.5f, 1.0f), player.flippedX, player.tint);
}