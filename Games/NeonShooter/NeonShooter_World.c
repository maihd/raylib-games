#include "NeonShooter_World.h"
#include "NeonShooter_Assets.h"
#include "NeonShooter_GameAudio.h"
#include "NeonShooter_ParticleSystem.h"

#include <stdlib.h>
#include <MaiLib.h>
#include <MaiMath.h>
#include <MaiArray.h>

#define DEFAULT_POINT_DAMPING 1.0F

static PointMass NewPointMass(vec2 position, float invMass)
{
    return (PointMass) {
        .position = position,
        .velocity = { 0, 0 },

        .acceleration = { 0, 0 }, 
        .invMass = invMass,
        .damping = DEFAULT_POINT_DAMPING,
    };
}

static void UpdatePointMass(PointMass* p, float timeStep)
{
    vec2 velocity = vec2Add(p->velocity, vec2Scale(p->acceleration, timeStep));
    vec2 position = vec2Add(p->position, vec2Scale(velocity, timeStep));

    if (vec2LengthSq(velocity) < 0.001f * 0.001f)
    {
        velocity = vec2New(0, 0);
    }
    else
    {
        velocity = vec2Scale(velocity, fmaxf(0.0f, 1.0f - p->damping * timeStep));
    }

    vec2 acceleration = p->acceleration;
    if (vec2LengthSq(acceleration) < 0.001f * 0.001f)
    {
        acceleration = vec2New(0, 0);
    }
    else
    {
        acceleration = vec2Scale(acceleration, fmaxf(0.0f, 1.0f - p->damping * timeStep));
    }

    p->position     = position;
    p->velocity     = velocity;
    p->acceleration = acceleration;
    p->damping      = DEFAULT_POINT_DAMPING;
}

static void PointMassApplyForce(PointMass* p, vec2 force, float timeStep)
{
    p->acceleration = vec2Add(p->acceleration, vec2Scale(force, p->invMass * timeStep));
}

static void PointMassIncreaseDamping(PointMass* p, float factor)
{
    p->damping = p->damping * factor;
}

static Spring NewSpring(PointMass* p0, PointMass* p1, float stiffness, float damping)
{
    return (Spring){
        .p0 = p0,
        .p1 = p1,

        .targetLength = vec2Distance(p0->position, p1->position) * 0.99f,
        .stiffness = stiffness,
        .damping = damping,
        .force = 280.0f,
    };
}

static void UpdateSpring(Spring* spring, float timeStep)
{
    vec2 diff = vec2Sub(spring->p0->position, spring->p1->position);
    float len = vec2Length(diff);
    if (len > spring->targetLength)
    {
        float changeRate = (len - spring->targetLength) / len;
        vec2 dvel = vec2Sub(spring->p1->velocity, spring->p0->velocity);
        vec2 force = vec2Sub(vec2Scale(diff, changeRate * spring->stiffness), vec2Scale(dvel, fmaxf(0.0f, 1.0f - spring->damping * timeStep)));

        PointMassApplyForce(spring->p0, vec2Scale(vec2Neg(force), spring->force), timeStep);
        PointMassApplyForce(spring->p1, vec2Scale(force, spring->force), timeStep);
    }
}

static WarpGrid NewWarpGrid(rect bounds, vec2 spacing)
{
    int cols = (int)(bounds.width / spacing.x) + 1;
    int rows = (int)(bounds.height / spacing.y) + 1;

    int pointCount = cols * rows;
    Array(Spring) springs = ArrayNew(Spring, 4 * pointCount);
    Array(PointMass) points = ArrayNew(PointMass, pointCount);
    Array(PointMass) fixedPoints = ArrayNew(PointMass, pointCount);

    if (!points || !springs || !fixedPoints)
    {
        ArrayFree(points);
        ArrayFree(springs);
        ArrayFree(fixedPoints);

        return (WarpGrid) { 0 };
    }

    ArraySetCount(points, pointCount);
    ArraySetCount(fixedPoints, pointCount);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int  index = i * cols + j;
            vec2 position = (vec2){ bounds.x + j * spacing.x, bounds.y + i * spacing.y };
            
            points[index] = NewPointMass(position, 1.0f);
            fixedPoints[index] = NewPointMass(position, 0.0f);

            if ((i == 0) || (j == 0) || (i == rows - 1) || (j == cols - 1)) // anchor the border of the grid
            {
                ArrayPush(springs, NewSpring(&fixedPoints[index], &points[index], 0.1f, 5.0f));
            }
            else if ((i % 3 == 0) && (j % 3 == 0)) // loosely anchor 1/9th of the point masses
            {
                ArrayPush(springs, NewSpring(&fixedPoints[index], &points[index], 0.002f, 40.0f));
            }

            const float stiffness = 0.28f;
            const float damping = 30.0f;
            if (j > 0)
            {
                ArrayPush(springs, NewSpring(&points[i * cols + (j - 1)], &points[index], stiffness, damping));
            }

            if (i > 0)
            {
                ArrayPush(springs, NewSpring(&points[(i - 1) * cols + j], &points[index], stiffness, damping));
            }
        }
    }

    return (WarpGrid) {
        .cols = cols, .rows = rows, .springs = springs, .points = points, .fixedPoints = fixedPoints
    };
}

static WarpGrid FreeWarpGrid(WarpGrid grid)
{
    ArrayFree(grid.points);
    ArrayFree(grid.springs);
    ArrayFree(grid.fixedPoints);

    return (WarpGrid) { 0 };
}

static void UpdateWarpGrid(WarpGrid grid, float timeStep)
{
    for (int i = 0, n = ArrayCount(grid.springs); i < n; i++)
    {
        UpdateSpring(&grid.springs[i], timeStep);
    }

    for (int i = 0, n = ArrayCount(grid.points); i < n; i++)
    {
        UpdatePointMass(&grid.points[i], timeStep);
    }
}

static void RenderWarpGrid(WarpGrid grid)
{
    BeginBlendMode(BLEND_ADDITIVE);

    Color color = (Color){ 30, 30, 139, 156 };   // dark blue

    int cols = grid.cols;
    int rows = grid.rows;
    for (int i = 1; i < rows; i++)
    {
        for (int j = 1; j < cols; j++)
        {
            float horThickness = 2.0f;
            float verThickness = 2.0f;

            vec2 current = grid.points[i * cols + j].position;
            
            vec2 left = grid.points[i * cols + (j - 1)].position;
            vec2 up = grid.points[(i - 1) * cols + j].position;

            vec2 midUp = vec2Scale(vec2Add(current, up), 0.5f);
            vec2 midLeft = vec2Scale(vec2Add(current, left), 0.5f);

            vec2 upLeft = grid.points[(i - 1) * cols + (j - 1)].position;
            //DrawLineEx(vec2Scale(vec2Add(upLeft, left), 0.5f), midLeft, horThickness, color);   // horizontal line

            int j0 = fmaxf(j - 2, 0);
            int j1 = fminf(j + 1, cols - 1);
            vec2 horMid = vec2CatmullRom(grid.points[i * cols + j0].position, left, current, grid.points[i * cols + j1].position, 0.5f);
            if (vec2DistanceSq(horMid, midLeft) > 1.0f)
            {
                DrawLineEx(left, horMid, horThickness, color);
                DrawLineEx(horMid, current, horThickness, color);
                DrawLineEx(vec2Scale(vec2Add(upLeft, up), 0.5f), horMid, verThickness, color);   // vertical line
            }
            else
            {
                DrawLineEx(left, current, horThickness, color);
                DrawLineEx(vec2Scale(vec2Add(upLeft, up), 0.5f), midLeft, verThickness, color);   // vertical line
            }
            
            int i0 = fmaxf(i - 2, 0);
            int i1 = fminf(i + 1, rows - 1);
            vec2 verMid = vec2CatmullRom(grid.points[i0 * cols + j].position, up, current, grid.points[i1 * cols + j].position, 0.5f);
            if (vec2DistanceSq(verMid, midUp) > 1.0f)
            {
                DrawLineEx(up, verMid, verThickness, color);
                DrawLineEx(verMid, current, verThickness, color);
                DrawLineEx(vec2Scale(vec2Add(upLeft, left), 0.5f), verMid, horThickness, color);   // horizontal line
            }
            else
            {
                DrawLineEx(up, current, verThickness, color);
                DrawLineEx(vec2Scale(vec2Add(upLeft, left), 0.5f), midUp, horThickness, color);   // horizontal line
            }
        }
    }

    EndBlendMode();
}

static void WarpGridApplyDirectedForce(WarpGrid grid, vec2 force, vec2 position, float radius, float timeStep)
{
    for (int i = 0, n = ArrayCount(grid.points); i < n; i++)
    {
        PointMass point = grid.points[i];
        if (vec2DistanceSq(position, point.position) < radius * radius)
        {
             PointMassApplyForce(&grid.points[i], vec2Scale(force, 1.0f / (1.0f + vec2DistanceSq(position, point.position))), timeStep);
        }
    }
}

static void WarpGridApplyImplosiveForce(WarpGrid grid, float force, vec2 position, float radius, float timeStep)
{
    for (int i = 0, n = ArrayCount(grid.points); i < n; i++)
    {
        PointMass point = grid.points[i];
        vec2 diff = vec2Sub(position, point.position);
        float distSq = vec2LengthSq(diff);
        if (distSq < radius * radius)
        {
            vec2 appliedForce = vec2Scale(diff, force * 50.0f / (1000.0f + distSq));

            PointMassApplyForce(&grid.points[i], appliedForce, timeStep);
            PointMassIncreaseDamping(&grid.points[i], 1.0f / 0.6f);
        }
    }
}

static void WarpGridApplyExplosiveForce(WarpGrid grid, float force, vec2 position, float radius, float timeStep)
{
    for (int i = 0, n = ArrayCount(grid.points); i < n; i++)
    {
        PointMass point = grid.points[i];
        vec2 diff = vec2Sub(point.position, position);
        float distSq = vec2LengthSq(diff);
        if (distSq < radius * radius)
        {
            vec2 appliedForce = vec2Scale(diff, force * 100.0f / (1000.0f + distSq));

            PointMassApplyForce(&grid.points[i], appliedForce, timeStep);
            PointMassIncreaseDamping(&grid.points[i], 1.0f / 0.6f);
        }
    }
}

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
    Texture texture = CacheTexture("Art/Bullet.png");
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
    GameAudioPlaySpawn();

    vec2 pos = GetSpawnPosition(*world);
    vec2 vel = vec2Normalize(vec2Sub(world->player.position, pos));

    Texture texture = CacheTexture("Art/Seeker.png");

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
    GameAudioPlaySpawn();

    vec2 pos = GetSpawnPosition(*world);
    vec2 vel = vec2Normalize(vec2Sub(world->player.position, pos));

    Texture texture = CacheTexture("Art/Wanderer.png");

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
    GameAudioPlaySpawn();

    vec2 pos = GetSpawnPosition(*world);
    vec2 vel = (vec2){ 0.0f, 0.0f };

    Texture texture = CacheTexture("Art/Black Hole.png");

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
        Texture texture = CacheTexture("Art/Laser.png");

        for (int i = 0; i < PARTICLE_COUNT; i++)
        {
            float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
            float angle = rand() % 101 / 100.0f * 2 * PI;
            vec2  vel   = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
            vec2  pos   = world->bullets.elements[index].position;
            vec4  color = (vec4){ 0.6f, 1.0f, 1.0f, 1.0f };

            SpawnParticle(texture, pos, color, 1.0f, vec2Repeat(1.0f), 0.0f, vel);
        }
    }
}

static void DestroySeeker(World* world, int index)
{
    GameAudioPlayExplosion();

    world->seekers.elements[index].active = false;
    FreeListCollect(world->seekers, index);

    Texture texture = CacheTexture("Art/Laser.png");

    float hue1 = rand() % 101 / 100.0f * 6.0f;
    float hue2 = fmodf(hue1 + (rand() % 101 / 100.0f * 2.0f), 6.0f);
    vec4  color1 = HSV(hue1, 0.5f, 1);
    vec4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 120; i++)
    {
        float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
        vec2  pos = world->seekers.elements[index].position;
        vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));

        SpawnParticle(texture, pos, color, 1.0f, (vec2){ 1.0f, 1.0f }, 0.0f, vel);
    }
}

void DestroyWanderer(World* world, int index)
{
    GameAudioPlayExplosion();

    world->wanderers.elements[index].active = false;
    FreeListCollect(world->wanderers, index);

    Texture texture = CacheTexture("Art/Laser.png");

    float hue1 = rand() % 101 / 100.0f * 6.0f;
    float hue2 = fmodf(hue1 + (rand() % 101 / 100.0f * 2.0f), 6.0f);
    vec4  color1 = HSV(hue1, 0.5f, 1);
    vec4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 120; i++)
    {
        float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
        vec2  pos = world->wanderers.elements[index].position;
        vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));

        SpawnParticle(texture, pos, color, 1.0f, (vec2){ 1.0f, 1.0f }, 0.0f, vel);
    }
}

void DestroyBlackhole(World* world, int index)
{
    GameAudioPlayExplosion();

    world->blackHoles.elements[index].active = false;
    FreeListCollect(world->blackHoles, index);

    Texture texture = CacheTexture("Art/Laser.png");

    float hue1 = rand() % 101 / 100.0f * 6.0f;
    float hue2 = fmodf(hue1 + (rand() % 101 / 100.0f * 2.0f), 6.0f);
    vec4  color1 = HSV(hue1, 0.5f, 1);
    vec4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 120; i++)
    {
        float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
        vec2  pos = world->blackHoles.elements[index].position;
        vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));

        SpawnParticle(texture, pos, color, 1.0f, (vec2){ 1.0f, 1.0f }, 0.0f, vel);
    }
}

void OnGameOver(World* world)
{
    GameAudioStopMusic();
    GameAudioPlayExplosion();

    FreeListClear(world->bullets);
    FreeListClear(world->seekers);
    FreeListClear(world->wanderers);
    FreeListClear(world->blackHoles);

    world->gameOverTimer = 3.0f;
    Texture texture = CacheTexture("Art/Laser.png");

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
        SpawnParticle(texture, world->player.position, color, world->gameOverTimer, vec2Repeat(1.0f), 0.0f, vel);
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

    world.grid = NewWarpGrid((rect) { -GetScreenWidth() * 1.1f, -GetScreenHeight() * 1.1f, 2.2f * GetScreenWidth(), 2.2f * GetScreenHeight() }, (vec2) { 64.0f, 64.0f });
    
    world.player.active = true;
    world.player.color = WHITE;
    world.player.position = (vec2) { 0.0f, 0.0f };
    world.player.rotation = 0.0f;
    world.player.scale = 1.0f;
    world.player.movespeed = 720.0f;
    world.player.texture = CacheTexture("Art/Player.png");
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
    world.gameOverTimer = 0.5f;

    world.bullets = FreeListNew(Entity, 256);
    world.seekers = FreeListNew(Entity, 256);
    world.wanderers = FreeListNew(Entity, 256);
    world.blackHoles = FreeListNew(Entity, 256);

    return world;
}

void WorldFree(World* world)
{
    FreeWarpGrid(world->grid);

    FreeListFree(world->bullets);
    FreeListFree(world->seekers);
    FreeListFree(world->wanderers);
    FreeListFree(world->blackHoles);

    *world = (World) { 0 };
}

void WorldUpdate(World* world, float horizontal, float vertical, vec2 aim_dir, bool fire, float dt)
{
    // Update warp grid
    UpdateWarpGrid(world->grid, dt);
    //UpdateMeshGrid(&world->meshGrid, dt);

    if (world->gameOverTimer > 0.0f)
    {
        world->gameOverTimer -= dt;
        if (world->gameOverTimer <= 0.0f)
        {
            GameAudioPlayMusic();
        }
        return;
    }

    // Update is in progress, locking the list
    world->lock = true;

    world->player.velocity = vec2Lerp(world->player.velocity, vec2Normalize((vec2){ horizontal, vertical }), 5.0f * dt);
    world->player = UpdateEntityWithBound(world->player, (vec2){ GetScreenWidth(), GetScreenHeight() }, dt);
    WarpGridApplyExplosiveForce(world->grid, 4.0f * world->player.movespeed, world->player.position, 50.0f, dt);
    if (vec2LengthSq(world->player.velocity) > 0.1f && fmodf(GetTotalTime(), 0.025f) <= 0.01f)
    {
        float speed;
        float angle = atan2f(world->player.velocity.y, world->player.velocity.x);
    
        Texture glow_tex = CacheTexture("Art/Laser.png");
        Texture line_tex = CacheTexture("Art/Laser.png");
    
        vec2 vel = vec2Scale(world->player.velocity, -0.25f * world->player.movespeed);
        vec2 pos = vec2Add(world->player.position, vec2Scale(world->player.velocity, -45.0f));
        vec2 nvel = vec2Scale(vec2New(vel.y, -vel.x), 0.9f * sinf(GetTotalTime() * 10.0f));
        float alpha = 0.7f;
    
        vec2 mid_vel = vel;
        SpawnParticle(glow_tex, pos, vec4Scale(vec4New(1.0f, 0.7f, 0.1f, 1.0f), alpha), 0.4f, vec2New(3.0f, 2.0f), angle, mid_vel);
        SpawnParticle(line_tex, pos, vec4Scale(vec4New(1.0f, 1.0f, 1.0f, 1.0f), alpha), 0.4f, vec2New(3.0f, 1.0f), angle, mid_vel);
    
        speed = rand() % 101 / 100.0f * 40.0f;
        angle = rand() % 101 / 100.0f * 2.0f * PI;
        vec2 side_vel1 = vec2Add(vel, vec2Add(nvel, vec2Scale(vec2New(cosf(angle), sinf(angle)), speed)));
        SpawnParticle(glow_tex, pos, vec4Scale(vec4New(0.8f, 0.2f, 0.1f, 1.0f), alpha), 0.4f, vec2New(3.0f, 2.0f), angle, side_vel1);
        SpawnParticle(line_tex, pos, vec4Scale(vec4New(1.0f, 1.0f, 1.0f, 1.0f), alpha), 0.4f, vec2New(3.0f, 1.0f), angle, side_vel1);
    
        speed = rand() % 101 / 100.0f * 40.0f;
        angle = rand() % 101 / 100.0f * 2.0f * PI;
        vec2 side_vel2 = vec2Sub(vel, vec2Add(nvel, vec2Scale(vec2New(cosf(angle), sinf(angle)), speed)));
        SpawnParticle(glow_tex, pos, vec4Scale(vec4New(0.8f, 0.2f, 0.1f, 1.0f), alpha), 0.4f, vec2New(3.0f, 2.0f), angle, side_vel2);
        SpawnParticle(line_tex, pos, vec4Scale(vec4New(1.0f, 1.0f, 1.0f, 1.0f), alpha), 0.4f, vec2New(3.0f, 1.0f), angle, side_vel2);
    }

    for (int i = 0, n = FreeListCount(world->bullets); i < n; i++)
    {
        if (world->bullets.elements[i].active)
        {
            Entity bullet = UpdateEntity(FreeListGet(world->bullets, i), dt);

            WarpGridApplyExplosiveForce(world->grid, 8000.0f, bullet.position, 64.0f, dt);

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
                //MeshGridApplyExplosiveForce(&world->meshGrid, 2.0f * s->movespeed, s->position, dt);
                WarpGridApplyExplosiveForce(world->grid, 4.0f * s->movespeed, s->position, 30.0f, dt);

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

                    WarpGridApplyExplosiveForce(world->grid, 4.0f * s->movespeed, s->position, 30.0f, dt);
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
            Texture glow_tex = CacheTexture("Art/Glow.png");
            Texture line_tex = CacheTexture("Art/Laser.png");

            vec4 color1 = (vec4){ 0.3f, 0.8f, 0.4f, 1.0f };
            vec4 color2 = (vec4){ 0.5f, 1.0f, 0.7f, 1.0f };

            if (GetFrameCount() % 3 == 0)
            {
                float speed = 16.0f * s->radius * (0.8f + (rand() % 101 / 100.0f) * 0.2f);
                float angle = rand() % 101 / 100.0f * GetTotalTime();
                vec2  vel = (vec2){ cosf(angle) * speed, sinf(angle) * speed };
                vec2  pos = vec2Add(vec2Add(s->position, vec2Scale((vec2){ vel.y, -vel.x }, 0.4f)), vec2Repeat(4.0f + rand() % 101 / 100.0f * 4.0f));

                vec4  color = vec4Add(color1, vec4Scale(vec4Sub(color2, color1), ((rand() % 101) / 100.0f)));
                SpawnParticle(glow_tex, pos, color, 4.0f, vec2New(0.3f, 0.2f), 0.0f, vel);
                SpawnParticle(line_tex, pos, color, 4.0f, vec2New(1.0f, 1.0f), 0.0f, vel);
            }

            if (GetFrameCount() % 60 == 0)
            {
                Texture texture = CacheTexture("Art/Laser.png");

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
                    SpawnParticle(texture, pos, color, 2.0f, vec2Repeat(1.0f), 0.0f, vec2Repeat(0.0f));
                }
            }

            if (s->color.a < 255)
            {
                int newValue = (int)fmaxf(255, s->color.a + dt * 255);
                s->color.a = newValue;
            }
            else
            {
                WarpGridApplyImplosiveForce(world->grid, 1500.0f, s->position, 1024.0f, dt);

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

        //GameAudioStopShoot();
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

            GameAudioPlayShoot();
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
    RenderWarpGrid(world.grid);
    //RenderMeshGrid(world.meshGrid);

    if (world.gameOverTimer > 0)
    {
        return;
    }

    RenderEntity(world.player);
    RenderEntities(world.bullets.elements);
    RenderEntities(world.seekers.elements);
    RenderEntities(world.wanderers.elements);
    RenderEntities(world.blackHoles.elements);
}

