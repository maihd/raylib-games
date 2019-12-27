#include <MaiLib.h>
#include <MaiMath.h>
#include <MaiArray.h>

const int DEFAULT_POINT_DAMPING = 3.0f;

typedef struct LiquidPoint2D
{
    vec2 position;
    vec2 velocity;
    vec2 acceleration;
    
    float invMass;
    float damping;
} LiquidPoint2D;

typedef struct LiquidSpring2D
{
    LiquidPoint2D* p0;
    LiquidPoint2D* p1;
 
    float targetLength;
    float stiffness;
    float damping;
    float force;
} LiquidSpring2D;

typedef struct LiquidSurface2D
{
    int cols;
    int rows;

    float cellWidth;
    float cellHeight;

    Array(LiquidPoint2D) points;
    Array(LiquidPoint2D) fixedPoints;

    Array(LiquidSpring2D) springs;
} LiquidSurface2D;

LiquidSurface2D NewLiquidSurface2D(rect bounds, vec2 spacing);
void            FreeLiquidSurface2D(LiquidSurface2D surface);

void            UpdateLiquidSurface2D(LiquidSurface2D surface, float dt);
void            RenderLiquidSurface2D(LiquidSurface2D surface, Color lineColor);

void            ApplyForceOnLiquidSurface2D(LiquidSurface2D surface, float force, vec2 position, float radius, float dt);

int main(void)
{
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Liquid Surface 2D");
    //SetTargetFPS(60);

    LiquidSurface2D surface = NewLiquidSurface2D((rect) { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, vec2New(16, 16));

    float timer     = 0.0f;
    float timeStep  = 1.0f / 60.0f;

    int   fpsCount    = 0;
    int   fpsValue    = 0;
    float fpsTimer    = 0.0f;
    float fpsInterval = 1.0f;

    while (!PollWindowEvents())
    {
        timer += GetDeltaTime();
        while (timer >= timeStep)
        {
            timer -= timeStep;
            fpsCount += 1;

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                ApplyForceOnLiquidSurface2D(surface, 12000.0f, GetMousePosition(), 80.0f, timeStep);
            }

            UpdateLiquidSurface2D(surface, timeStep);
        }

        fpsTimer += GetDeltaTime();
        if (fpsTimer >= fpsInterval)
        {
            fpsTimer -= fpsInterval;
            
            fpsValue = fpsCount;
            fpsCount = 0;
        }

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            RenderLiquidSurface2D(surface, GRAY);

            DrawText(TextFormat("CPU FPS: %d", fpsValue), 0, 0, 24, DARKGREEN);
            DrawText(TextFormat("GPU FPS: %d", GetFPS()), 0, 30, 24, DARKGREEN);
        }
        EndDrawing();
    }

    FreeLiquidSurface2D(surface);
    CloseWindow();
    return 0;
}

LiquidPoint2D NewLiquidPoint2D(vec2 position, float invMass)
{
    return (LiquidPoint2D) {
        position, vec2New(0, 0), vec2New(0, 0), invMass, DEFAULT_POINT_DAMPING
    };
}

LiquidSpring2D NewLiquidSpring2D(LiquidPoint2D* p0, LiquidPoint2D* p1, float stiffness, float damping)
{
    return (LiquidSpring2D) {
        p0, p1,

        vec2Distance(p0->position, p1->position) * 0.95f,
        stiffness, damping, 250.0f
    };
}

LiquidSurface2D NewLiquidSurface2D(rect bounds, vec2 spacing)
{
    int cols = (int)(bounds.width / spacing.x) + 2;
    int rows = (int)(bounds.height / spacing.y) + 2;

    int pointCount = cols * rows;
    Array(LiquidPoint2D) points = ArrayNew(LiquidPoint2D, pointCount);
    Array(LiquidPoint2D) fixedPoints = ArrayNew(LiquidPoint2D, pointCount);
    Array(LiquidSpring2D) springs = ArrayNew(LiquidSpring2D, 4 * pointCount);

    ArraySetCount(points, pointCount);
    ArraySetCount(fixedPoints, pointCount);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int index = i * cols + j;

            vec2 position = vec2New(bounds.x + j * spacing.x, bounds.y + i * spacing.y);
            points[index] = NewLiquidPoint2D(position, 1.0f);
            fixedPoints[index] = NewLiquidPoint2D(position, 1.0f);

            if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1)
            {
                ArrayPush(springs, NewLiquidSpring2D(&fixedPoints[index], &points[index], 0.2f, 4.0f));
            }
            else if (i % 3 == 0 && j % 3 == 0)
            {
                ArrayPush(springs, NewLiquidSpring2D(&fixedPoints[index], &points[index], 0.004f, 40.0f));
            }

            const float stiffness = 0.28f;
            const float damping = 20.0f;
            if (j > 0)
            {
                ArrayPush(springs, NewLiquidSpring2D(&points[i * cols + (j - 1)], &points[index], stiffness, damping));
            }

            if (i > 0)
            {
                ArrayPush(springs, NewLiquidSpring2D(&points[(i - 1) * cols + j], &points[index], stiffness, damping));
            }
        }
    }

    return (LiquidSurface2D) {
        cols, rows, spacing.x, spacing.y,

        points, fixedPoints, springs
    };
}

void FreeLiquidSurface2D(LiquidSurface2D surface)
{
    ArrayFree(surface.points);
    ArrayFree(surface.fixedPoints);
    ArrayFree(surface.springs);
}

void UpdateLiquidSurface2D(LiquidSurface2D surface, float timeStep)
{
    for (int i = 0, n = ArrayCount(surface.springs); i < n; i++)
    {
        LiquidSpring2D spring = surface.springs[i];

        vec2 diff = vec2Sub(spring.p0->position, spring.p1->position);
        float len = vec2Length(diff);
        if (len > spring.targetLength)
        {
            float changeRate = (len - spring.targetLength) / len;
            vec2 diffVelocity = vec2Sub(spring.p1->velocity, spring.p0->velocity);
            vec2 force = vec2Sub(vec2Scale(diff, spring.stiffness * changeRate), vec2Scale(diffVelocity, fmaxf(0.0f, 1.0f - spring.damping * timeStep)));
            
            spring.p0->acceleration = vec2Add(spring.p0->acceleration, vec2Scale(vec2Neg(force), spring.force * spring.p0->invMass * timeStep));
            spring.p1->acceleration = vec2Add(spring.p1->acceleration, vec2Scale(force, spring.force * spring.p1->invMass * timeStep));
        }
    }

    for (int i = 0, n = ArrayCount(surface.points); i < n; i++)
    {
        LiquidPoint2D point = surface.points[i];

        point.velocity = vec2Add(point.velocity, vec2Scale(point.acceleration, timeStep));
        point.position = vec2Add(point.position, vec2Scale(point.velocity, timeStep));

        point.acceleration = vec2Scale(point.acceleration, fmaxf(0.0f, 1.0f - point.damping * timeStep));
        point.velocity = vec2Scale(point.velocity, fmaxf(0.0f, 1.0f - point.damping * timeStep));

        point.damping = DEFAULT_POINT_DAMPING;

        surface.points[i] = point;
    }
}

void RenderLiquidSurface2D(LiquidSurface2D surface, Color lineColor)
{
    int cols = surface.cols;
    int rows = surface.rows;

    for (int i = 1; i < rows; i++)
    {
        for (int j = 1; j < cols; j++)
        {
            vec2 p0 = surface.points[i * cols + j].position;
            vec2 p1 = surface.points[(i - 1) * cols + j].position;
            vec2 p2 = surface.points[i * cols + (j - 1)].position;

            DrawLineEx(p0, p1, 1.0f, lineColor);
            DrawLineEx(p0, p2, 1.0f, lineColor);
        }
    }
}

void ApplyForceOnLiquidSurface2D(LiquidSurface2D surface, float force, vec2 position, float radius, float timeStep)
{
    for (int i = 0, n = ArrayCount(surface.points); i < n; i++)
    {
        LiquidPoint2D point = surface.points[i];
        vec2  diff = vec2Sub(point.position, position);
        float distSq = vec2LengthSq(diff);
        if (distSq < radius * radius)
        {
            float accuratedForce = 100.0f * force / (10000.0f + distSq);
            vec2  directionForce = vec2Scale(diff, accuratedForce);

            point.acceleration = vec2Add(point.acceleration, vec2Scale(directionForce, point.invMass * timeStep));
            point.damping = point.damping * (1.0f / 0.6f);
            
            surface.points[i] = point;
        }
    }
}
