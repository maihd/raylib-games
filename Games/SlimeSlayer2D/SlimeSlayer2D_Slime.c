#include <MaiLib.h>
#include <MaiMath.h>

#include "SlimeSlayer2D_Slime.h"

Slime SlimeNew(vec2 position, bool originFlippedX, float moveSpeed, Color tint,
    TileAnimation idleAnimation,
    TileAnimation walkAnimation,
    TileAnimation hurtAnimation,
    TileAnimation attackAnimation,
    TileAnimation dieAnimation)
{
    return (Slime) {
        .state = SLIME_IDLE,

        .flippedX = false,
        .originFlippedX = originFlippedX,

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

void SlimeFree(Slime slime)
{
    UnloadTileAnimation(slime.idleAnimation);
    UnloadTileAnimation(slime.walkAnimation);
    UnloadTileAnimation(slime.hurtAnimation);
    UnloadTileAnimation(slime.attackAnimation);
    UnloadTileAnimation(slime.dieAnimation);
}

SlimeOutput SlimeUpdate(Slime* slime, SlimeInput input, float timeStep)
{
    SlimeOutput output = { 0 };

    Slime newSlime = *slime;
    UpdateTileAnimation(&newSlime.animation, timeStep);

    switch (newSlime.state)
    {
    case SLIME_IDLE:
        if (input.hurt)
        {
            newSlime.state = SLIME_HURT;
            newSlime.direction = input.hurtDirection;
            newSlime.animation = newSlime.hurtAnimation;
        }
        else if (input.attack)
        {
            newSlime.state = SLIME_ATTACK;
            newSlime.direction = 0.0f;
            newSlime.animation = newSlime.attackAnimation;
        }
        if (fabsf(input.moveDirection) > 0.0f)
        {
            newSlime.state = SLIME_WALK;
            newSlime.direction = input.moveDirection;
            newSlime.flippedX = input.moveDirection < 0.0f;
            newSlime.animation = newSlime.walkAnimation;
        }
        break;

    case SLIME_WALK:
        if (input.hurt)
        {
            newSlime.state = SLIME_HURT;
            newSlime.direction = input.hurtDirection;
            newSlime.animation = newSlime.hurtAnimation;
        }
        else if (input.attack)
        {
            newSlime.state = SLIME_ATTACK;
            newSlime.direction = 0.0f;
            newSlime.animation = newSlime.attackAnimation;
        }
        else if (fabsf(input.moveDirection) == 0.0f)
        {
            newSlime.state = SLIME_IDLE;
            newSlime.direction = input.moveDirection;
            newSlime.animation = newSlime.idleAnimation;
        }
        else
        {
            newSlime.direction = input.moveDirection;
            newSlime.flippedX = input.moveDirection < 0.0f;
            newSlime.position = vec2Add(newSlime.position, vec2New(newSlime.direction * newSlime.moveSpeed * timeStep, 0.0f));
        }
        break;

    case SLIME_ATTACK:
        if (input.hurt)
        {
            newSlime.state = SLIME_HURT;
            newSlime.direction = input.hurtDirection;
            newSlime.animation = newSlime.hurtAnimation;
        }
        else if (!newSlime.animation.running)
        {
            if (input.attack)
            {
                newSlime.state = SLIME_ATTACK;
                newSlime.direction = 0.0f;
                newSlime.animation = newSlime.attackAnimation;
            }
            else if (fabsf(input.moveDirection) > 0.0f)
            {
                newSlime.state = SLIME_WALK;
                newSlime.direction = input.moveDirection;
                newSlime.flippedX = input.moveDirection < 0.0f;
                newSlime.animation = newSlime.walkAnimation;
            }
            else
            {
                newSlime.state = SLIME_IDLE;
                newSlime.direction = input.moveDirection;
                newSlime.animation = newSlime.idleAnimation;
            }
        }
        else
        {
            if (newSlime.animation.timer >= newSlime.attackDelay)
            {
                output.hitboxingAttack = true;
            }
        }
        break;

    case SLIME_HURT:
        if (!newSlime.animation.running)
        {
            //if (input.attack)
            //{
            //    newSlime.state     = SLIME_ATTACK;
            //    newSlime.direction = 0.0f;
            //    newSlime.animation = newSlime.attackAnimation;
            //}
            //else if (fabsf(input.moveDirection) > 0.0f)
            //{
            //    newSlime.state     = SLIME_WALK;
            //    newSlime.direction = input.moveDirection;
            //    newSlime.flippedX  = input.moveDirection < 0.0f;
            //    newSlime.animation = newSlime.walkAnimation;
            //}
            //else
            //{
            //    newSlime.state     = SLIME_IDLE;
            //    newSlime.direction = input.moveDirection;
            //    newSlime.animation = newSlime.idleAnimation;
            //}
            newSlime.state = SLIME_DIE;
            newSlime.animation = newSlime.dieAnimation;
        }
        else
        {
            newSlime.flippedX = newSlime.direction > 0.0f;
            newSlime.position = vec2Add(newSlime.position, vec2New(newSlime.direction * newSlime.moveSpeed * timeStep, 0.0f));
        }
        break;

    case SLIME_DIE:
        // Ignore all input
        if (newSlime.animation.running)
        {
        }
        else
        {
            output.removeSelf = true;
        }
        break;
    }

    *slime = newSlime;

    return output;
}

void SlimeRender(Slime slime)
{
    bool flippedX = slime.originFlippedX - slime.flippedX;
    DrawTileAnimation(slime.animation, slime.position, vec2New(0.5f, 1.0f), flippedX, slime.tint);
}