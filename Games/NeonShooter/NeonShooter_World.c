#include "NeonShooter_World.h"

#include <MaiMath.h>
#include <MaiArray.h>

#define ASSET_PATH(path) "../Games/NeonShooter/Assets/" path

static inline vec4 HSV(float h, float s, float v)
{
    if (h == 0 && s == 0)
        return (vec4){ v, v, v, 1.0f };

    float c = s * v;
    float x = c * (1 - fabsf(fmodf(h, 2) - 1));
    float m = v - c;

    if (h < 1)      return (vec4){ c + m, x + m, m, 1.0f };
    else if (h < 2) return (vec4){ x + m, c + m, m, 1.0f };
    else if (h < 3) return (vec4){ m, c + m, x + m, 1.0f };
    else if (h < 4) return (vec4){ m, x + m, c + m, 1.0f };
    else if (h < 5) return (vec4){ x + m, m, c + m, 1.0f };
    else            return (vec4){ c + m, m, x + m, 1.0f };
}

static Entity UpdateEntity(Entity entity, float dt)
{
    return (Entity){
        .active = entity.active,

        .scale = entity.scale,
        .position = vec2Add(entity.position, vec2Scale(entity.velocity, entity.movespeed * dt)),
        .rotation = entity.velocity.x != 0.0f || entity.velocity.y != 0.0f ? atan2f(entity.velocity.y, entity.velocity.x) : entity.rotation,

        .velocity = entity.velocity,
        .movespeed = entity.movespeed,

        .color = entity.color,
        .radius = entity.radius,

        .texture = entity.texture,
    };
}

static void UpdateEntities(Array(Entity) entities, float dt)
{
    for (int i = 0, n = ArrayCount(entities); i < n; i++)
    {
        entities[i] = UpdateEntity(entities[i], dt);
    }
}

static Entity UpdateEntityWithBound(Entity entity, vec2 bound, float dt)
{
    vec2  pos = vec2Add(entity.position, vec2Scale(entity.velocity, entity.movespeed * dt));
    float rad = entity.radius;

    if (pos.x + rad > bound.x)
    {
        pos.x = bound.x - rad;
    }
    else if (pos.x - rad < -bound.x)
    {
        pos.x = rad - bound.x;
    }

    if (pos.y + rad > bound.y)
    {
        pos.y = bound.y - rad;
    }
    else if (pos.y - rad < -bound.y)
    {
        pos.y = rad - bound.y;
    }

    return (Entity){
        entity.active,

        entity.scale,
        entity.velocity.x != 0.0f || entity.velocity.y != 0.0f ? atan2f(entity.velocity.y, entity.velocity.x) : entity.rotation,
        pos,

        entity.velocity,
        entity.movespeed,

        entity.color,
        entity.radius,

        entity.texture,
    };
}

static void UpdateEntitiesWithBound(Array(Entity) entities, vec2 bound, float dt)
{
    for (int i = 0, n = ArrayCount(entities); i < n; i++)
    {
        entities[i] = UpdateEntityWithBound(entities[i], bound, dt);
    }
}

static void RenderEntity(Entity entity)
{
    if (entity.active)
    {
        //DrawTextureEx(entity.texture, entity.position, entity.rotation * RAD2DEG, entity.scale, entity.color
        DrawTexturePro(
            entity.texture, 
            (rect) { 0, 0, entity.texture.width, entity.texture.height }, 
            (rect) { entity.position.x, entity.position.y, entity.texture.width, entity.texture.height },
            (vec2) { entity.texture.width * 0.5f, entity.texture.height * 0.5f },
            entity.rotation * RAD2DEG, 
            entity.color
        );
    }
}

static void RenderEntities(Array(Entity) entities)
{
    for (int i = 0, n = ArrayCount(entities); i < n; i++)
    {
        Entity entity = entities[i];
        if (entity.active)
        {
            DrawTexturePro(
                entity.texture, 
                (rect) { 0, 0, entity.texture.width, entity.texture.height }, 
                (rect) { entity.position.x, entity.position.y, entity.texture.width, entity.texture.height },
                (vec2) { entity.texture.width * 0.5f, entity.texture.height * 0.5f },
                entity.rotation * RAD2DEG, 
                entity.color
            );
            //DrawTextureEx(entity.texture, entity.position, entity.rotation * RAD2DEG, entity.scale, entity.color);
        }
    }
}

static void SpawnBullet(World* world, vec2 pos, vec2 vel)
{
    Texture texture = LoadTexture(ASSET_PATH("Art/Bullet.png"));
    Entity entity = {
        true,
        1.0f,
        atan2f(vel.y, vel.x),
        pos,

        vel,
        1280.0f,

        WHITE,
        texture.height * 0.5f,
        texture,
    };

    FreeListAdd(world->bullets, entity);
}

static void FireBullets(World* world, vec2 aim_dir)
{
    float angle = atan2f(aim_dir.y, aim_dir.x) + (rand() % 101) / 100.0f * (PI * 0.025f);
    float offset = PI * 0.1f;

    aim_dir = (vec2){ cosf(angle), sinf(angle) };

    // First bullet
    {
        vec2 vel = vec2Normalize(aim_dir);
        vec2 pos = vec2Add(world->player.position, vec2Scale((vec2){ cosf(angle + offset), sinf(angle + offset) }, (float)world->player.texture.width * 1.25f));
        SpawnBullet(world, pos, vel);
    }

    // Second bullet
    {
        vec2 vel = vec2Normalize(aim_dir);
        vec2 pos = vec2Add(world->player.position, vec2Scale((vec2){ cosf(angle - offset), sinf(angle - offset) }, (float)world->player.texture.width * 1.25f));
        SpawnBullet(world, pos, vel);
    }
}

static vec2 GetSpawnPosition(World world)
{
    const float min_distance_sqr = (GetScreenHeight() * 0.3f) * (GetScreenHeight() * 0.3f);

    vec2 pos;
    do
    {
        float x = (2.0f * (rand() % 101) / 100.0f - 1.0f) * 0.8f * GetScreenWidth();
        float y = (2.0f * (rand() % 101) / 100.0f - 1.0f) * 0.8f * GetScreenHeight();
        pos = (vec2){ x, y };
    } while (vec2DistanceSq(pos, world.player.position) < min_distance_sqr);

    return pos;
}

static void SpawnSeeker(World* world)
{
    //GameAudio::PlaySpawn();

    vec2 pos = GetSpawnPosition(*world);
    vec2 vel = vec2Normalize(vec2Sub(world->player.position, pos));

    Texture texture = LoadTexture(ASSET_PATH("Art/Seeker.png"));

    Entity entity = {
        true,

        1.0f,
        atan2f(vel.y, vel.x),
        pos,

        vel,
        360.0f,

        Fade(WHITE, 0.0f),
        texture.width * 0.5f,
        texture
    };

    FreeListAdd(world->seekers, entity);
}

static void SpawnWanderer(World* world)
{
    //GameAudio::PlaySpawn();

    vec2 pos = GetSpawnPosition(*world);
    vec2 vel = vec2Normalize(vec2Sub(world->player.position, pos));

    Texture texture = LoadTexture(ASSET_PATH("Art/Wanderer.png"));

    Entity entity = {
        true,

        1.0f,
        atan2f(vel.y, vel.x),
        pos,

        vel,
        240.0f,

        Fade(WHITE, 0.0f),
        texture.width * 0.5f,
        texture
    };

    FreeListAdd(world->wanderers, entity);
}

static void SpawnBlackhole(World* world)
{
    //GameAudio::PlaySpawn();

    vec2 pos = GetSpawnPosition(*world);
    vec2 vel = (vec2){ 0.0f, 0.0f };

    Texture texture = LoadTexture(ASSET_PATH("Art/Black Hole.png"));

    Entity entity = {
        true,

        1.0f,
        atan2f(vel.y, vel.x),
        pos,

        vel,
        240.0f,

        Fade(WHITE, 0.0f),
        texture.width * 0.5f,
        texture
    };

    FreeListAdd(world->blackHoles, entity);
}

static void DestroyBullet(World* world, int index, bool explosion)
{
    world->bullets.elements[index].active = false;
    FreeListCollect(world->bullets, index);

    if (explosion)
    {
        const int PARTICLE_COUNT = 30;
        Texture texture = LoadTexture(ASSET_PATH("Art/Laser.png"));

        for (int i = 0; i < PARTICLE_COUNT; i++)
        {
            float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
            float angle = rand() % 101 / 100.0f * 2 * PI;
            vec2  vel   = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
            vec4  color = (vec4){ 0.6f, 1.0f, 1.0f, 1.0f };

            //ParticleSystem::SpawnParticle(texture, bullet->position, color, 1.0f, float2(1.0f), 0.0f, vel);
        }
    }
}

static void DestroySeeker(World* world, int index)
{
    //GameAudio::PlayExplosion();

    world->seekers.elements[index].active = false;
    FreeListCollect(world->seekers, index);

    Texture texture = LoadTexture(ASSET_PATH("Art/Laser.png"));

    float hue1 = rand() % 101 / 100.0f * 6.0f;
    float hue2 = fmodf(hue1 + (rand() % 101 / 100.0f * 2.0f), 6.0f);
    vec4  color1 = HSV(hue1, 0.5f, 1);
    vec4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 120; i++)
    {
        float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
        vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));

        //ParticleSystem::SpawnParticle(texture, world->seekers.elements[index].position, color, 1.0f, vec2{ 1.0f, 1.0f }, 0.0f, vel);
    }
}

void DestroyWanderer(World* world, int index)
{
    //GameAudio::PlayExplosion();

    world->wanderers.elements[index].active = false;
    FreeListCollect(world->wanderers, index);

    Texture texture = LoadTexture(ASSET_PATH("Art/Laser.png"));

    float hue1 = rand() % 101 / 100.0f * 6.0f;
    float hue2 = fmodf(hue1 + (rand() % 101 / 100.0f * 2.0f), 6.0f);
    vec4  color1 = HSV(hue1, 0.5f, 1);
    vec4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 120; i++)
    {
        float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
        vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));

        //ParticleSystem::SpawnParticle(texture, world->seekers.elements[index].position, color, 1.0f, vec2{ 1.0f, 1.0f }, 0.0f, vel);
    }
}

void DestroyBlackhole(World* world, int index)
{
    //GameAudio::PlayExplosion();

    world->blackHoles.elements[index].active = false;
    FreeListCollect(world->blackHoles, index);

    Texture texture = LoadTexture(ASSET_PATH("Art/Laser.png"));

    float hue1 = rand() % 101 / 100.0f * 6.0f;
    float hue2 = fmodf(hue1 + (rand() % 101 / 100.0f * 2.0f), 6.0f);
    vec4  color1 = HSV(hue1, 0.5f, 1);
    vec4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 120; i++)
    {
        float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
        vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));

        //ParticleSystem::SpawnParticle(texture, world->seekers.elements[index].position, color, 1.0f, vec2{ 1.0f, 1.0f }, 0.0f, vel);
    }
}

void OnGameOver(World* world)
{
    //GameAudio::PlayExplosion();

    FreeListClear(world->bullets);
    FreeListClear(world->seekers);
    FreeListClear(world->wanderers);
    FreeListClear(world->blackHoles);

    world->gameOverTimer = 3.0f;
    Texture texture = LoadTexture(ASSET_PATH("Art/Laser.png"));

    float hue1 = rand() % 101 / 100.0f * 6.0f;
    float hue2 = fmodf(hue1 + (rand() % 101 / 100.0f * 2.0f), 6.0f);
    vec4  color1 = HSV(hue1, 0.5f, 1);
    vec4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 1200; i++)
    {
        float speed = 10.0f * fmaxf((float)GetScreenWidth(), (float)GetScreenHeight()) * (0.6f + (rand() % 101 / 100.0f) * 0.4f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };

        vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));
        //ParticleSystem::SpawnParticle(texture, player.position, color, gameOverTimer, float2(1.0f), 0.0f, vel);
    }

    world->player.position = (vec2){ 0, 0 };
    world->player.velocity = (vec2){ 0, 0 };
    world->player.rotation = 0.0f;
}

bool UpdateBlackhole(Entity* blackhole, Entity* other)
{
    if (vec2Distance(other->position, blackhole->position) <= other->radius + blackhole->radius)
    {
        return true;
    }
    else if (vec2Distance(other->position, blackhole->position) <= other->radius + blackhole->radius * 10.0f)
    {
        vec2 diff = vec2Sub(blackhole->position, other->position);
        other->velocity = vec2Add(other->velocity, vec2Scale(vec2Normalize(diff), lerpf(1.0f, 0.0f, vec2Length(diff) / (blackhole->radius * 10.0f))));
        other->velocity = vec2Normalize(other->velocity);
    }

    return false;
}

World WorldNew(void)
{
    World world = { 0 };

    world.player.active = true;
    world.player.color = WHITE;
    world.player.position = (vec2) { 0.0f, 0.0f };
    world.player.rotation = 0.0f;
    world.player.scale = 1.0f;
    world.player.movespeed = 720.0f;
    world.player.texture = LoadTexture(ASSET_PATH("Art/Player.png"));
    world.player.radius = world.player.texture.width * 0.5f;

    world.seekerSpawnRate = 80;
    world.wandererSpawnRate = 60;
    world.blackHoleSpawnRate = 20;

    world.oldFire = false;
    world.fireTimer = 0.0f;
    world.fireInterval = 0.1f;

    world.spawnTimer = 0.0f;
    world.spawnInterval = 1.0f;

    world.lock = false;
    world.gameOverTimer = 0.0f;

    world.bullets = FreeListNew(Entity, 256);
    world.seekers = FreeListNew(Entity, 256);
    world.wanderers = FreeListNew(Entity, 256);
    world.blackHoles = FreeListNew(Entity, 256);

    return world;
}

void WorldFree(World* world)
{
    FreeListFree(world->bullets);
    FreeListFree(world->seekers);
    FreeListFree(world->wanderers);
    FreeListFree(world->blackHoles);

    *world = (World) { 0 };
}

void WorldUpdate(World* world, float horizontal, float vertical, vec2 aim_dir, bool fire, float dt)
{
    if (world->gameOverTimer > 0.0f)
    {
        world->gameOverTimer -= dt;
        return;
    }

    // Update is in progress, locking the list
    world->lock = true;

    world->player.velocity = vec2Lerp(world->player.velocity, vec2Normalize((vec2){ horizontal, vertical }), 5.0f * dt);
    world->player = UpdateEntityWithBound(world->player, (vec2){ GetScreenWidth(), GetScreenHeight() }, dt);
    //if (lensqr(world->player.velocity) > 0.1f && fmodf(Time::GetTotalTime(), 0.025f) <= 0.01f)
    //{
    //    float speed;
    //    float angle = atan2f(player.velocity.y, player.velocity.x);
    //
    //    Texture glow_tex = LoadTexture(ASSET_PATH("Art/Laser.png");
    //    Texture line_tex = LoadTexture(ASSET_PATH("Art/Laser.png");
    //
    //    float2 vel = -0.25f * player.movespeed * player.velocity;
    //    float2 pos = player.position + 45.0f * (-player.velocity);
    //    float2 nvel = float2(vel.y, -vel.x) * 0.9f * sinf(Game::totalTime * 10.0f);
    //    float alpha = 0.7f;
    //
    //    float2 mid_vel = vel;
    //    ParticleSystem::SpawnParticle(glow_tex, pos, float4(1.0f, 0.7f, 0.1f, 1.0f) * alpha, 0.4f, float2(3.0f, 2.0f), angle, mid_vel);
    //    ParticleSystem::SpawnParticle(line_tex, pos, float4(1.0f, 1.0f, 1.0f, 1.0f) * alpha, 0.4f, float2(3.0f, 1.0f), angle, mid_vel);
    //
    //    speed = rand() % 101 / 100.0f * 40.0f;
    //    angle = rand() % 101 / 100.0f * 2.0f * PI;
    //    float2 side_vel1 = vel + nvel + float2(cosf(angle), sinf(angle)) * speed;
    //    ParticleSystem::SpawnParticle(glow_tex, pos, float4(0.8f, 0.2f, 0.1f, 1.0f) * alpha, 0.4f, float2(3.0f, 2.0f), angle, side_vel1);
    //    ParticleSystem::SpawnParticle(line_tex, pos, float4(1.0f, 1.0f, 1.0f, 1.0f) * alpha, 0.4f, float2(3.0f, 1.0f), angle, side_vel1);
    //
    //    speed = rand() % 101 / 100.0f * 40.0f;
    //    angle = rand() % 101 / 100.0f * 2.0f * PI;
    //    float2 side_vel2 = vel - nvel + float2(cosf(angle), sinf(angle)) * speed;
    //    ParticleSystem::SpawnParticle(glow_tex, pos, float4(0.8f, 0.2f, 0.1f, 1.0f) * alpha, 0.4f, float2(3.0f, 2.0f), angle, side_vel2);
    //    ParticleSystem::SpawnParticle(line_tex, pos, float4(1.0f, 1.0f, 1.0f, 1.0f) * alpha, 0.4f, float2(3.0f, 1.0f), angle, side_vel2);
    //}

    for (int i = 0, n = FreeListCount(world->bullets); i < n; i++)
    {
        if (world->bullets.elements[i].active)
        {
            Entity bullet = UpdateEntity(FreeListGet(world->bullets, i), dt);
            if (bullet.position.x < -GetScreenWidth()
                || bullet.position.x > GetScreenWidth()
                || bullet.position.y < -GetScreenHeight()
                || bullet.position.y > GetScreenHeight())
            {
                DestroyBullet(world, i, true);
            }
            else
            {
                FreeListSet(world->bullets, i, bullet);
            }
        }
    }

    for (int i = 0, n = FreeListCount(world->seekers); i < n; i++)
    {
        Entity* s = FreeListRef(world->seekers, i);
        if (s->active)
        {
            if (s->color.a < 255)
            {
                int newValue = (int)fmaxf(255, s->color.a + dt * 255);
                s->color.a = newValue;
            }
            else
            {
                vec2 dir = vec2Normalize(vec2Sub(world->player.position, s->position));
                vec2 acl = vec2Scale(dir, 10.0f * dt);
                s->velocity = vec2Normalize(vec2Add(s->velocity, acl));
                *s = UpdateEntity(*s, dt);
            }
        }
    }

    for (int i = 0, n = FreeListCount(world->wanderers); i < n; i++)
    {
        Entity* s = FreeListRef(world->wanderers, i);
        if (s->active)
        {
            if (s->color.a < 255)
            {
                int newValue = (int)fmaxf(255, s->color.a + dt * 255);
                s->color.a = newValue;
            }
            else
            {
                const int INTERPOLATIONS = 6;
                const float real_speed = s->movespeed / INTERPOLATIONS;

                float direction = atan2f(s->velocity.y, s->velocity.x);
                for (int j = 0; j < INTERPOLATIONS; j++)
                {
                    direction += (0.12f * (rand() % 101 / 100.0f) - 0.06f) * PI;

                    if (s->position.x < -GetScreenWidth() || s->position.x > GetScreenWidth()
                        || s->position.y < -GetScreenHeight() || s->position.y > GetScreenHeight())
                    {
                        direction = atan2f(-s->position.y, -s->position.x) + (1.0f * (rand() % 101 / 100.0f) - 0.5f) * PI;
                    }

                    s->rotation = direction;
                    s->velocity = (vec2){ cosf(direction), sinf(direction) };
                    s->position = vec2Add(s->position, vec2Scale(s->velocity, real_speed * dt));
                }
            }
        }
    }

    for (int i = 0, n = FreeListCount(world->bullets); i < n; i++)
    {
        Entity* b = &world->bullets.elements[i];

        if (!b->active) continue;
        for (int j = 0, m = FreeListCount(world->seekers); j < m; j++)
        {
            Entity* s = &world->seekers.elements[j];
            if (!s->active || s->color.a < 255) continue;

            if (vec2Distance(b->position, s->position) <= b->radius + s->radius)
            {
                DestroyBullet(world, i, true);
                DestroySeeker(world, j);
                break;
            }
        }

        if (!b->active) continue;
        for (int j = 0, m = FreeListCount(world->wanderers); j < m; j++)
        {
            Entity* s = &world->wanderers.elements[j];
            if (!s->active || s->color.a < 255) continue;

            if (vec2Distance(b->position, s->position) <= b->radius + s->radius)
            {
                DestroyBullet(world, i, true);
                DestroyWanderer(world, j);
                break;
            }
        }

        if (!b->active) continue;
        for (int j = 0, m = FreeListCount(world->blackHoles); j < m; j++)
        {
            Entity* s = &world->blackHoles.elements[j];
            if (!s->active || s->color.a < 255) continue;

            float d = vec2Distance(b->position, s->position);
            if (d <= b->radius + s->radius)
            {
                DestroyBullet(world, i, true);
                DestroyBlackhole(world, j);
                break;
            }
            else if (d <= b->radius + s->radius * 5.0f)
            {
                float r = b->radius + s->radius * 5.0f;
                float t = (d - r) / r;
                b->velocity = vec2Normalize(vec2Add(b->velocity, vec2Scale(vec2Normalize(vec2Sub(b->position, s->position)), 0.3f)));
            }
        }
    }

    for (int j = 0, m = FreeListCount(world->seekers); j < m; j++)
    {
        Entity* s = &world->seekers.elements[j];
        if (!s->active || s->color.a < 255) continue;

        if (vec2Distance(world->player.position, s->position) <= world->player.radius + s->radius)
        {
            OnGameOver(world);
            break;
        }
    }

    for (int j = 0, m = FreeListCount(world->wanderers); j < m; j++)
    {
        Entity* s = &world->wanderers.elements[j];
        if (!s->active || s->color.a < 255) continue;

        if (vec2Distance(world->player.position, s->position) <= world->player.radius + s->radius)
        {
            OnGameOver(world);
            break;
        }
    }

    for (int i = 0, n = FreeListCount(world->blackHoles); i < n; i++)
    {
        Entity* s = &world->blackHoles.elements[i];
        if (s->active)
        {
            Texture glow_tex = LoadTexture(ASSET_PATH("Art/Glow.png"));
            Texture line_tex = LoadTexture(ASSET_PATH("Art/Laser.png"));

            vec4 color1 = (vec4){ 0.3f, 0.8f, 0.4f, 1.0f };
            vec4 color2 = (vec4){ 0.5f, 1.0f, 0.7f, 1.0f };

            //if (GetFrameTime() % 3 == 0)
            if (false)
            {
                float speed = 16.0f * s->radius * (0.8f + (rand() % 101 / 100.0f) * 0.2f);
                float angle = rand() % 101 / 100.0f * 0.0f;//Time::GetTotalTime();
                vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
                vec2  pos = vec2Add(vec2Add(s->position, vec2Scale((vec2){ vel.y, -vel.x }, 0.4f)), vec2Repeat(4.0f + rand() % 101 / 100.0f * 4.0f));

                vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));
                //ParticleSystem::SpawnParticle(glow_tex, pos, color, 4.0f, float2(0.3f, 0.2f), 0.0f, vel);
                //ParticleSystem::SpawnParticle(line_tex, pos, color, 4.0f, float2(1.0f, 1.0f), 0.0f, vel);
            }

            //if (Time::GetTotalFrames() % 60 == 0)
            if (false)
            {
                Texture texture = LoadTexture(ASSET_PATH("Art/Laser.png"));

                float hue1 = rand() % 101 / 100.0f * 6.0f;
                float hue2 = fmodf(hue1 + (rand() % 101 / 100.0f * 2.0f), 6.0f);
                vec4  color1 = HSV(hue1, 0.5f, 1);
                vec4  color2 = HSV(hue2, 0.5f, 1);

                for (int i = 0; i < 120.0f; i++)
                {
                    float speed = 180.0f;
                    float angle = rand() % 101 / 100.0f * 2 * PI;
                    vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
                    vec2  pos = vec2Add(s->position, vel);
                    vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));
                    //ParticleSystem::SpawnParticle(texture, pos, color, 2.0f, float2(1.0f), 0.0f, float2(0.0f));
                }
            }

            if (s->color.a < 255)
            {
                int newValue = (int)fmaxf(255, s->color.a + dt * 255);
                s->color.a = newValue;
            }
            else
            {
                if (UpdateBlackhole(s, &world->player))
                {
                    OnGameOver(world);
                    break;
                }

                for (int j = 0, m = FreeListCount(world->seekers); j < m; j++)
                {
                    Entity* other = &world->seekers.elements[j];
                    if (!other->active || other->color.a < 255) continue;

                    if (UpdateBlackhole(s, other))
                    {
                        DestroySeeker(world, j);
                        break;
                    }
                }

                for (int j = 0, m = FreeListCount(world->wanderers); j < m; j++)
                {
                    Entity* other = &world->wanderers.elements[j];
                    if (!other->active || other->color.a < 255) continue;

                    if (UpdateBlackhole(s, other))
                    {
                        DestroyWanderer(world, j);
                        break;
                    }
                }
            }
        }
    }

    // Update is done, unlock the list
    world->lock = false;

    // Fire bullet if requested
    if (!fire)
    {
        world->oldFire = false;
        world->fireTimer = 0.0f;

        //GameAudio::StopShoot();
    }
    else
    {
        if (world->oldFire != fire)
        {
            world->oldFire = fire;
            world->fireTimer = world->fireInterval;
        }

        world->fireTimer += dt;
        if (world->fireTimer >= world->fireInterval)
        {
            world->fireTimer = 0;
            FireBullets(world, aim_dir);

            //GameAudio::PlayShoot();
        }
    }

    // Spawn enemies
    world->spawnTimer += dt;
    if (world->spawnTimer >= world->spawnInterval)
    {
        world->spawnTimer -= world->spawnInterval;

        if (rand() % 101 < world->seekerSpawnRate) SpawnSeeker(world);
        if (rand() % 101 < world->wandererSpawnRate) SpawnWanderer(world);
        if (rand() % 101 < world->blackHoleSpawnRate) SpawnBlackhole(world);
    }
}

void WorldRender(World world)
{
    if (world.gameOverTimer > 0)
    {
        return;
    }

    Camera2D camera = {
        (vec2) {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f},
        vec2Zero(),
        0,
        0.5f,
    };
    BeginMode2D(camera);
    {
        RenderEntity(world.player);
        RenderEntities(world.bullets.elements);
        RenderEntities(world.seekers.elements);
        RenderEntities(world.wanderers.elements);
        RenderEntities(world.blackHoles.elements);
    }
    EndMode2D();
}

