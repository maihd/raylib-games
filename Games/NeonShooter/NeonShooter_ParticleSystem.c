#include "NeonShooter_ParticleSystem.h"
#include "NeonShooter_World.h"

#include <MaiLib.h>
#include <MaiMath.h>
#include <MaiArray.h>
#include <MaiFreeList.h>

typedef struct Particle
{
    bool       active;
    Texture    texture;
    vec2       velocity;
    vec2       position;
    float      rotation;
    vec2       scale;
    vec4       color;
    float      timer;
    float      duration;
} Particle;

FreeListStruct(Particle);
static FreeList(Particle) particles;

void InitParticles(void)
{
    particles = FreeListNew(Particle, 1024);
}

void ClearParticles(void)
{
    FreeListClear(particles);
}

void ReleaseParticles(void)
{
    FreeListFree(particles);
}

void SpawnParticle(Texture texture, vec2 position, vec4 color, float duration, vec2 scale, float theta, vec2 velocity)
{
    Particle particle = {
        .active = true,
        .scale = scale,
        .rotation = theta,
        .position = position,
        .velocity = velocity,
        .texture = texture,
        .color = color,
        .timer = 0.0f,
        .duration = duration
    };

    FreeListAdd(particles, particle);
}

static bool UpdateParticle(World* world, Particle* p, float dt)
{
    bool result = false;

    if (p->active)
    {
        p->timer += dt;
        if (p->timer >= p->duration)
        {
            result = true;
            p->active = false;
        }

        p->rotation = atan2f(p->velocity.y, p->velocity.x);
        p->position = vec2Add(p->position, vec2Scale(p->velocity, dt));
        p->velocity = vec2Scale(p->velocity, 1.0f - 3 * dt);

        p->scale.x = 1.0f - p->timer / p->duration;
        p->color.w = 1.0f - p->timer / p->duration;

        if (p->position.x <= -GetScreenWidth())
        {
            p->velocity.x = fabsf(p->velocity.x);
            p->position.x = -GetScreenWidth();
        }
        else if (p->position.x >= GetScreenWidth())
        {
            p->velocity.x = -fabsf(p->velocity.x);
            p->position.x = GetScreenWidth();
        }

        if (p->position.y <= -GetScreenHeight())
        {
            p->velocity.y = fabsf(p->velocity.y);
            p->position.y = -GetScreenHeight();
        }
        else if (p->position.y >= GetScreenHeight())
        {
            p->velocity.y = -fabsf(p->velocity.y);
            p->position.y = GetScreenHeight();
        }

        for (int i = 0, n = FreeListCount(world->blackHoles); i < n; i++)
        {
            Entity* blackhole = &world->blackHoles.elements[i];
            if (!blackhole->active) continue;

            vec2 diff = vec2Sub(blackhole->position, p->position);
            float d = vec2Length(diff);
            vec2 normal = vec2Normalize(diff);
            p->velocity = vec2Add(p->velocity, vec2Scale(normal, fmaxf(0.0f, GetScreenWidth() / d)));

            // add tangential acceleration for nearby particles
            if (d < 10.0f * blackhole->radius)
            {
                p->velocity = vec2Add(p->velocity, vec2Scale(vec2New(normal.y, -normal.x), (21.0f * blackhole->radius / (120.0f + 1.2f * d))));
            }
        }
    }

    return result;
}

void UpdateParticles(World* world, float dt)
{
    for (int i = 0, n = FreeListCount(particles); i < n; i++)
    {
        Particle* p = &particles.elements[i];
        if (UpdateParticle(world, p, dt))
        {
            FreeListCollect(particles, i);
        }
    }
}

void DrawParticles()
{
    BeginBlendMode(BLEND_ADDITIVE);
    for (int i = 0, n = FreeListCount(particles); i < n; i++)
    {
        Particle p = particles.elements[i];
        if (p.active)
        {
            Color color = ColorFromVec4(p.color);
            DrawTexturePro(
                p.texture, 
                (rect) { 0, 0, p.texture.width, p.texture.height }, 
                (rect) { p.position.x, p.position.y, p.texture.width * p.scale.x, p.texture.height * p.scale.y },
                (vec2) { p.texture.width * 0.5f, p.texture.height * 0.5f },
                p.rotation * RAD2DEG, 
                color
            );
        }
    }
    EndBlendMode();
}