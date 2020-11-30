#include "NeonShooter_World.h"
#include "NeonShooter_Assets.h"
#include "NeonShooter_GameAudio.h"
#include "NeonShooter_ParticleSystem.h"

#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

#define DEFAULT_POINT_DAMPING 1.0F

int GetFrameCount(void);

static float clampf(float value, float min, float max)
{
    const float res = value < min ? min : value;
    return res > max ? max : res;
}

// Calculate linear interpolation between two floats
static float lerpf(float start, float end, float amount)
{
    return start + amount * (end - start);
}

static float Vector2LengthSq(Vector2 v)
{
    return v.x * v.x + v.y * v.y;
}

static float Vector2DistanceSq(Vector2 a, Vector2 b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float d  = dx * dx + dy * dy;
    return d;
}

static Vector2 Vector2CatmullRom(Vector2 v1, Vector2 v2, Vector2 v3, Vector2 v4, float amount)
{
    float squared = amount * amount;
    float cubed = amount * squared;

    float x = 0.5f * ((((2.0f * v2.x) + ((-v1.x + v3.x) * amount))
        + (((((2.0f * v1.x) - (5.0f * v2.x)) + (4.0f * v3.x)) - v4.x) * squared))
        + ((((-v1.x + (3.0f * v2.x)) - (3.0f * v3.x)) + v4.x) * cubed));

    float y = 0.5f * ((((2.0f * v2.y) + ((-v1.y + v3.y) * amount))
        + (((((2.0f * v1.y) - (5.0f * v2.y)) + (4.0f * v3.y)) - v4.y) * squared))
        + ((((-v1.y + (3.0f * v2.y)) - (3.0f * v3.y)) + v4.y) * cubed));

    return (Vector2) { x, y };
}

static Vector4 Vector4Add(Vector4 v1, Vector4 v2)
{
    Vector4 result = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v1.w };
    return result;
}

static Vector4 Vector4Subtract(Vector4 v1, Vector4 v2)
{
    Vector4 result = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v1.w };
    return result;
}

static Vector4 Vector4Scale(Vector4 v, float scalar)
{
    Vector4 result = { v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar };
    return result;
}

static PointMass NewPointMass(Vector2 position, float invMass)
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
    Vector2 velocity = Vector2Add(p->velocity, Vector2Scale(p->acceleration, timeStep));
    Vector2 position = Vector2Add(p->position, Vector2Scale(velocity, timeStep));

    if (Vector2LengthSq(velocity) < 0.001f * 0.001f)
    {
        velocity = (Vector2) { 0, 0 };
    }
    else
    {
        velocity = Vector2Scale(velocity, fmaxf(0.0f, 1.0f - p->damping * timeStep));
    }

    Vector2 acceleration = p->acceleration;
    if (Vector2LengthSq(acceleration) < 0.001f * 0.001f)
    {
        acceleration = (Vector2) { 0, 0 };
    }
    else
    {
        acceleration = Vector2Scale(acceleration, fmaxf(0.0f, 1.0f - p->damping * timeStep));
    }

    p->position     = position;
    p->velocity     = velocity;
    p->acceleration = acceleration;
    p->damping      = DEFAULT_POINT_DAMPING;
}

static void PointMassApplyForce(PointMass* p, Vector2 force, float timeStep)
{
    p->acceleration = Vector2Add(p->acceleration, Vector2Scale(force, p->invMass * timeStep));
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

        .targetLength = Vector2Distance(p0->position, p1->position) * 0.99f,
        .stiffness = stiffness,
        .damping = damping,
        .force = 60.0f,
    };
}

static void UpdateSpring(Spring* spring, float timeStep)
{
    Vector2 diff = Vector2Subtract(spring->p0->position, spring->p1->position);
    float len = Vector2Length(diff);
    if (len > spring->targetLength)
    {
        float changeRate = (len - spring->targetLength) / len;
        Vector2 dvel = Vector2Subtract(spring->p1->velocity, spring->p0->velocity);
        Vector2 force = Vector2Subtract(Vector2Scale(diff, changeRate * spring->stiffness), Vector2Scale(dvel, fmaxf(0.0f, 1.0f - spring->damping * timeStep)));

        PointMassApplyForce(spring->p0, Vector2Scale(Vector2Negate(force), spring->force), timeStep);
        PointMassApplyForce(spring->p1, Vector2Scale(force, spring->force), timeStep);
    }
}

static WarpGrid NewWarpGrid(Rectangle bounds, Vector2 spacing)
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
            Vector2 position = (Vector2){ bounds.x + j * spacing.x, bounds.y + i * spacing.y };
            
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

            Vector2 current = grid.points[i * cols + j].position;
            
            Vector2 left = grid.points[i * cols + (j - 1)].position;
            Vector2 up = grid.points[(i - 1) * cols + j].position;

            Vector2 midUp = Vector2Scale(Vector2Add(current, up), 0.5f);
            Vector2 midLeft = Vector2Scale(Vector2Add(current, left), 0.5f);

            Vector2 upLeft = grid.points[(i - 1) * cols + (j - 1)].position;
            //DrawLineEx(Vector2Scale(Vector2Add(upLeft, left), 0.5f), midLeft, horThickness, color);   // horizontal line

            int j0 = fmaxf(j - 2, 0);
            int j1 = fminf(j + 1, cols - 1);
            Vector2 horMid = Vector2CatmullRom(grid.points[i * cols + j0].position, left, current, grid.points[i * cols + j1].position, 0.5f);
            if (Vector2DistanceSq(horMid, midLeft) > 1.0f)
            {
                DrawLineEx(left, horMid, horThickness, color);
                DrawLineEx(horMid, current, horThickness, color);
                DrawLineEx(Vector2Scale(Vector2Add(upLeft, up), 0.5f), horMid, verThickness, color);   // vertical line
            }
            else
            {
                DrawLineEx(left, current, horThickness, color);
                DrawLineEx(Vector2Scale(Vector2Add(upLeft, up), 0.5f), midLeft, verThickness, color);   // vertical line
            }
            
            int i0 = fmaxf(i - 2, 0);
            int i1 = fminf(i + 1, rows - 1);
            Vector2 verMid = Vector2CatmullRom(grid.points[i0 * cols + j].position, up, current, grid.points[i1 * cols + j].position, 0.5f);
            if (Vector2DistanceSq(verMid, midUp) > 1.0f)
            {
                DrawLineEx(up, verMid, verThickness, color);
                DrawLineEx(verMid, current, verThickness, color);
                DrawLineEx(Vector2Scale(Vector2Add(upLeft, left), 0.5f), verMid, horThickness, color);   // horizontal line
            }
            else
            {
                DrawLineEx(up, current, verThickness, color);
                DrawLineEx(Vector2Scale(Vector2Add(upLeft, left), 0.5f), midUp, horThickness, color);   // horizontal line
            }
        }
    }

    EndBlendMode();
}

static void WarpGridApplyDirectedForce(WarpGrid grid, Vector2 force, Vector2 position, float radius, float timeStep)
{
    for (int i = 0, n = ArrayCount(grid.points); i < n; i++)
    {
        PointMass point = grid.points[i];
        if (Vector2DistanceSq(position, point.position) < radius * radius)
        {
             PointMassApplyForce(&grid.points[i], Vector2Scale(force, 1.0f / (1.0f + Vector2DistanceSq(position, point.position))), timeStep);
        }
    }
}

static void WarpGridApplyImplosiveForce(WarpGrid grid, float force, Vector2 position, float radius, float timeStep)
{
    for (int i = 0, n = ArrayCount(grid.points); i < n; i++)
    {
        PointMass point = grid.points[i];
        Vector2 diff = Vector2Subtract(position, point.position);
        float distSq = Vector2LengthSq(diff);
        if (distSq < radius * radius)
        {
            Vector2 appliedForce = Vector2Scale(diff, force * 50.0f / (1000.0f + distSq));

            PointMassApplyForce(&grid.points[i], appliedForce, timeStep);
            PointMassIncreaseDamping(&grid.points[i], 1.0f / 0.6f);
        }
    }
}

static void WarpGridApplyExplosiveForce(WarpGrid grid, float force, Vector2 position, float radius, float timeStep)
{
    for (int i = 0, n = ArrayCount(grid.points); i < n; i++)
    {
        PointMass point = grid.points[i];
        Vector2 diff = Vector2Subtract(point.position, position);
        float distSq = Vector2LengthSq(diff);
        if (distSq < radius * radius)
        {
            Vector2 appliedForce = Vector2Scale(diff, force * 100.0f / (1000.0f + distSq));

            PointMassApplyForce(&grid.points[i], appliedForce, timeStep);
            PointMassIncreaseDamping(&grid.points[i], 1.0f / 0.6f);
        }
    }
}

static inline Vector4 HSV(float h, float s, float v)
{
    if (h == 0 && s == 0)
        return (Vector4){ v, v, v, 1.0f };

    float c = s * v;
    float x = c * (1 - fabsf(fmodf(h, 2) - 1));
    float m = v - c;

    if (h < 1)      return (Vector4){ c + m, x + m, m, 1.0f };
    else if (h < 2) return (Vector4){ x + m, c + m, m, 1.0f };
    else if (h < 3) return (Vector4){ m, c + m, x + m, 1.0f };
    else if (h < 4) return (Vector4){ m, x + m, c + m, 1.0f };
    else if (h < 5) return (Vector4){ x + m, m, c + m, 1.0f };
    else            return (Vector4){ c + m, m, x + m, 1.0f };
}

static Entity UpdateEntity(Entity entity, float dt)
{
    return (Entity){
        .active = entity.active,

        .scale = entity.scale,
        .position = Vector2Add(entity.position, Vector2Scale(entity.velocity, entity.movespeed * dt)),
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

static Entity UpdateEntityWithBound(Entity entity, Vector2 bound, float dt)
{
    Vector2  pos = Vector2Add(entity.position, Vector2Scale(entity.velocity, entity.movespeed * dt));
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

static void UpdateEntitiesWithBound(Array(Entity) entities, Vector2 bound, float dt)
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
            (Rectangle) { 0, 0, entity.texture.width, entity.texture.height }, 
            (Rectangle) { entity.position.x, entity.position.y, entity.texture.width, entity.texture.height },
            (Vector2) { entity.texture.width * 0.5f, entity.texture.height * 0.5f },
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
                (Rectangle) { 0, 0, entity.texture.width, entity.texture.height }, 
                (Rectangle) { entity.position.x, entity.position.y, entity.texture.width, entity.texture.height },
                (Vector2) { entity.texture.width * 0.5f, entity.texture.height * 0.5f },
                entity.rotation * RAD2DEG, 
                entity.color
            );
            //DrawTextureEx(entity.texture, entity.position, entity.rotation * RAD2DEG, entity.scale, entity.color);
        }
    }
}

static void SpawnBullet(World* world, Vector2 pos, Vector2 vel)
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

static void FireBullets(World* world, Vector2 aim_dir)
{
    float angle = atan2f(aim_dir.y, aim_dir.x) + (rand() % 101) / 100.0f * (PI * 0.025f);
    float offset = PI * 0.1f;

    aim_dir = (Vector2){ cosf(angle), sinf(angle) };

    // First bullet
    {
        Vector2 vel = Vector2Normalize(aim_dir);
        Vector2 pos = Vector2Add(world->player.position, Vector2Scale((Vector2){ cosf(angle + offset), sinf(angle + offset) }, (float)world->player.texture.width * 1.25f));
        SpawnBullet(world, pos, vel);
    }

    // Second bullet
    {
        Vector2 vel = Vector2Normalize(aim_dir);
        Vector2 pos = Vector2Add(world->player.position, Vector2Scale((Vector2){ cosf(angle - offset), sinf(angle - offset) }, (float)world->player.texture.width * 1.25f));
        SpawnBullet(world, pos, vel);
    }
}

static Vector2 GetSpawnPosition(World world)
{
    const float min_distance_sqr = (GetScreenHeight() * 0.3f) * (GetScreenHeight() * 0.3f);

    Vector2 pos;
    do
    {
        float x = (2.0f * (rand() % 101) / 100.0f - 1.0f) * 0.8f * GetScreenWidth();
        float y = (2.0f * (rand() % 101) / 100.0f - 1.0f) * 0.8f * GetScreenHeight();
        pos = (Vector2){ x, y };
    } while (Vector2DistanceSq(pos, world.player.position) < min_distance_sqr);

    return pos;
}

static void SpawnSeeker(World* world)
{
    GameAudioPlaySpawn();

    Vector2 pos = GetSpawnPosition(*world);
    Vector2 vel = Vector2Normalize(Vector2Subtract(world->player.position, pos));

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

    Vector2 pos = GetSpawnPosition(*world);
    Vector2 vel = Vector2Normalize(Vector2Subtract(world->player.position, pos));

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

    Vector2 pos = GetSpawnPosition(*world);
    Vector2 vel = (Vector2){ 0.0f, 0.0f };

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
            Vector2  vel   = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
            Vector2  pos   = world->bullets.elements[index].position;
            Vector4  color = (Vector4){ 0.6f, 1.0f, 1.0f, 1.0f };

            SpawnParticle(texture, pos, color, 1.0f, (Vector2) { 1.0f, 1.0f }, 0.0f, vel);
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
    Vector4  color1 = HSV(hue1, 0.5f, 1);
    Vector4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 120; i++)
    {
        float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        Vector2  vel = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
        Vector2  pos = world->seekers.elements[index].position;
        Vector4  color = Vector4Add(color1, Vector4Scale(Vector4Subtract(color2, color1), ((rand() % 101) / 100.0f)));

        SpawnParticle(texture, pos, color, 1.0f, (Vector2){ 1.0f, 1.0f }, 0.0f, vel);
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
    Vector4  color1 = HSV(hue1, 0.5f, 1);
    Vector4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 120; i++)
    {
        float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        Vector2  vel = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
        Vector2  pos = world->wanderers.elements[index].position;
        Vector4  color = Vector4Add(color1, Vector4Scale(Vector4Subtract(color2, color1), ((rand() % 101) / 100.0f)));

        SpawnParticle(texture, pos, color, 1.0f, (Vector2){ 1.0f, 1.0f }, 0.0f, vel);
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
    Vector4  color1 = HSV(hue1, 0.5f, 1);
    Vector4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 120; i++)
    {
        float speed = 640.0f * (0.2f + (rand() % 101 / 100.0f) * 0.8f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        Vector2  vel = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
        Vector2  pos = world->blackHoles.elements[index].position;
        Vector4  color = Vector4Add(color1, Vector4Scale(Vector4Subtract(color2, color1), ((rand() % 101) / 100.0f)));

        SpawnParticle(texture, pos, color, 1.0f, (Vector2){ 1.0f, 1.0f }, 0.0f, vel);
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
    Vector4  color1 = HSV(hue1, 0.5f, 1);
    Vector4  color2 = HSV(hue2, 0.5f, 1);

    for (int i = 0; i < 1200; i++)
    {
        float speed = 10.0f * fmaxf((float)GetScreenWidth(), (float)GetScreenHeight()) * (0.6f + (rand() % 101 / 100.0f) * 0.4f);
        float angle = rand() % 101 / 100.0f * 2 * PI;
        Vector2  vel = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };

        Vector4  color = Vector4Add(color1, Vector4Scale(Vector4Subtract(color2, color1), ((rand() % 101) / 100.0f)));
        SpawnParticle(texture, world->player.position, color, world->gameOverTimer, (Vector2) { 1.0f, 1.0f }, 0.0f, vel);
    }

    world->player.position = (Vector2){ 0, 0 };
    world->player.velocity = (Vector2){ 0, 0 };
    world->player.rotation = 0.0f;
}

bool UpdateBlackhole(Entity* blackhole, Entity* other)
{
    if (Vector2Distance(other->position, blackhole->position) <= other->radius + blackhole->radius)
    {
        return true;
    }
    else if (Vector2Distance(other->position, blackhole->position) <= other->radius + blackhole->radius * 10.0f)
    {
        Vector2 diff = Vector2Subtract(blackhole->position, other->position);
        other->velocity = Vector2Add(other->velocity, Vector2Scale(Vector2Normalize(diff), lerpf(1.0f, 0.0f, Vector2Length(diff) / (blackhole->radius * 10.0f))));
        other->velocity = Vector2Normalize(other->velocity);
    }

    return false;
}

World WorldNew(void)
{
    World world = { 0 };

    world.grid = NewWarpGrid((Rectangle) { -GetScreenWidth() * 1.1f, -GetScreenHeight() * 1.1f, 2.2f * GetScreenWidth(), 2.2f * GetScreenHeight() }, (Vector2) { 128.0f, 128.0f });
    
    world.player.active = true;
    world.player.color = WHITE;
    world.player.position = (Vector2) { 0.0f, 0.0f };
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

void WorldUpdate(World* world, float horizontal, float vertical, Vector2 aim_dir, bool fire, float dt)
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
   
    Vector2 moveDirection;
    if (horizontal == 0.0f && vertical == 0.0f)
    {
        moveDirection = (Vector2) { 0.0f, 0.0f };
    }
    else
    {
        moveDirection = Vector2Normalize((Vector2) { horizontal, vertical });
    }

    world->player.velocity = Vector2Lerp(world->player.velocity, moveDirection, 5.0f * dt);
    world->player = UpdateEntityWithBound(world->player, (Vector2){ GetScreenWidth(), GetScreenHeight() }, dt);
    WarpGridApplyExplosiveForce(world->grid, 4.0f * world->player.movespeed, world->player.position, 50.0f, dt);
    if (Vector2LengthSq(world->player.velocity) > 0.1f && fmodf(GetTime(), 0.025f) <= 0.01f)
    {
        float speed;
        float angle = atan2f(world->player.velocity.y, world->player.velocity.x);
    
        Texture glow_tex = CacheTexture("Art/Laser.png");
        Texture line_tex = CacheTexture("Art/Laser.png");
    
        Vector2 vel = Vector2Scale(world->player.velocity, -0.25f * world->player.movespeed);
        Vector2 pos = Vector2Add(world->player.position, Vector2Scale(world->player.velocity, -45.0f));
        Vector2 nvel = Vector2Scale((Vector2) { vel.y, -vel.x }, 0.9f * sinf(GetTime() * 10.0f));
        float alpha = 0.7f;
    
        Vector2 mid_vel = vel;
        SpawnParticle(glow_tex, pos, Vector4Scale((Vector4) { 1.0f, 0.7f, 0.1f, 1.0f }, alpha), 0.4f, (Vector2) { 3.0f, 2.0f }, angle, mid_vel);
        SpawnParticle(line_tex, pos, Vector4Scale((Vector4) { 1.0f, 1.0f, 1.0f, 1.0f }, alpha), 0.4f, (Vector2) { 3.0f, 1.0f }, angle, mid_vel);
    
        speed = rand() % 101 / 100.0f * 40.0f;
        angle = rand() % 101 / 100.0f * 2.0f * PI;
        Vector2 side_vel1 = Vector2Add(vel, Vector2Add(nvel, Vector2Scale((Vector2) { cosf(angle), sinf(angle) }, speed)));
        SpawnParticle(glow_tex, pos, Vector4Scale((Vector4) { 0.8f, 0.2f, 0.1f, 1.0f }, alpha), 0.4f, (Vector2) { 3.0f, 2.0f }, angle, side_vel1);
        SpawnParticle(line_tex, pos, Vector4Scale((Vector4) { 1.0f, 1.0f, 1.0f, 1.0f }, alpha), 0.4f, (Vector2) { 3.0f, 1.0f }, angle, side_vel1);
    
        speed = rand() % 101 / 100.0f * 40.0f;
        angle = rand() % 101 / 100.0f * 2.0f * PI;
        Vector2 side_vel2 = Vector2Subtract(vel, Vector2Add(nvel, Vector2Scale((Vector2) { cosf(angle), sinf(angle) }, speed)));
        SpawnParticle(glow_tex, pos, Vector4Scale((Vector4) { 0.8f, 0.2f, 0.1f, 1.0f }, alpha), 0.4f, (Vector2) { 3.0f, 2.0f }, angle, side_vel2);
        SpawnParticle(line_tex, pos, Vector4Scale((Vector4) { 1.0f, 1.0f, 1.0f, 1.0f }, alpha), 0.4f, (Vector2) { 3.0f, 1.0f }, angle, side_vel2);
    }

    for (int i = 0, n = FreeListCount(world->bullets); i < n; i++)
    {
        if (world->bullets.elements[i].active)
        {
            Entity bullet = UpdateEntity(FreeListGet(world->bullets, i), dt);

            WarpGridApplyExplosiveForce(world->grid, 4000.0f, bullet.position, 128.0f, dt);

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

                Vector2 dir = Vector2Normalize(Vector2Subtract(world->player.position, s->position));
                Vector2 acl = Vector2Scale(dir, 10.0f * dt);
                s->velocity = Vector2Normalize(Vector2Add(s->velocity, acl));
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
                    s->velocity = (Vector2){ cosf(direction), sinf(direction) };
                    s->position = Vector2Add(s->position, Vector2Scale(s->velocity, real_speed * dt));

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

            if (Vector2Distance(b->position, s->position) <= b->radius + s->radius)
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

            if (Vector2Distance(b->position, s->position) <= b->radius + s->radius)
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

            float d = Vector2Distance(b->position, s->position);
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
                b->velocity = Vector2Normalize(Vector2Add(b->velocity, Vector2Scale(Vector2Normalize(Vector2Subtract(b->position, s->position)), 0.3f)));
            }
        }
    }

    for (int j = 0, m = FreeListCount(world->seekers); j < m; j++)
    {
        Entity* s = &world->seekers.elements[j];
        if (!s->active || s->color.a < 255) continue;

        if (Vector2Distance(world->player.position, s->position) <= world->player.radius + s->radius)
        {
            OnGameOver(world);
            break;
        }
    }

    for (int j = 0, m = FreeListCount(world->wanderers); j < m; j++)
    {
        Entity* s = &world->wanderers.elements[j];
        if (!s->active || s->color.a < 255) continue;

        if (Vector2Distance(world->player.position, s->position) <= world->player.radius + s->radius)
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

            Vector4 color1 = (Vector4){ 0.3f, 0.8f, 0.4f, 1.0f };
            Vector4 color2 = (Vector4){ 0.5f, 1.0f, 0.7f, 1.0f };

            
            if (GetFrameCount() % 3 == 0)
            {
                float speed = 16.0f * s->radius * (0.8f + (rand() % 101 / 100.0f) * 0.2f);
                float angle = rand() % 101 / 100.0f * GetTime();
                float value = 4.0f + rand() % 101 / 100.0f * 4.0f;
                Vector2  vel = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
                Vector2  pos = Vector2Add(Vector2Add(s->position, Vector2Scale((Vector2) { vel.y, -vel.x }, 0.4f)), (Vector2) { value, value });

                Vector4  color = Vector4Add(color1, Vector4Scale(Vector4Subtract(color2, color1), ((rand() % 101) / 100.0f)));
                SpawnParticle(glow_tex, pos, color, 4.0f, (Vector2) { 0.3f, 0.2f }, 0.0f, vel);
                SpawnParticle(line_tex, pos, color, 4.0f, (Vector2) { 1.0f, 1.0f }, 0.0f, vel);
            }

            if (GetFrameCount() % 60 == 0)
            {
                Texture texture = CacheTexture("Art/Laser.png");

                float hue1 = rand() % 101 / 100.0f * 6.0f;
                float hue2 = fmodf(hue1 + (rand() % 101 / 100.0f * 2.0f), 6.0f);
                Vector4  color1 = HSV(hue1, 0.5f, 1);
                Vector4  color2 = HSV(hue2, 0.5f, 1);

                for (int i = 0; i < 120.0f; i++)
                {
                    float speed = 180.0f;
                    float angle = rand() % 101 / 100.0f * 2 * PI;
                    Vector2  vel = (Vector2){ cosf(angle) * speed, sinf(angle) * speed };
                    Vector2  pos = Vector2Add(s->position, vel);
                    Vector4  color = Vector4Add(color1, Vector4Scale(Vector4Subtract(color2, color1), ((rand() % 101) / 100.0f)));
                    SpawnParticle(texture, pos, color, 2.0f, (Vector2) { 1.0f, 1.0f }, 0.0f, (Vector2) { 0.0f, 0.0f });
                }
            }

            if (s->color.a < 255)
            {
                int newValue = (int)fmaxf(255, s->color.a + dt * 255);
                s->color.a = newValue;
            }
            else
            {
                WarpGridApplyImplosiveForce(world->grid, 2000.0f, s->position, 1024.0f, dt);

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

