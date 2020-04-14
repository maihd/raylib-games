#include <raylib.h>
#include <raymath.h>
#include <Array.h>

#include <assert.h>

const int DEFAULT_POINT_DAMPING = 3.0f;

typedef struct LiquidPoint2D
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    
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

bool            IsLiquidSurface2DValid(LiquidSurface2D surface);

LiquidSurface2D NewLiquidSurface2D(Rectangle bounds, Vector2 spacing);
void            FreeLiquidSurface2D(LiquidSurface2D surface);

void            UpdateLiquidSurface2D(LiquidSurface2D surface, float dt);
void            RenderLiquidSurface2D(LiquidSurface2D surface, Color lineColor);

void            ApplyForceOnLiquidSurface2D(LiquidSurface2D surface, float force, Vector2 position, float radius, float dt);

int main(void)
{
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Liquid Surface 2D");
    //SetTargetFPS(60);

    LiquidSurface2D surface = NewLiquidSurface2D((Rectangle) { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, (Vector2) { 16, 16 });
    if (!IsLiquidSurface2DValid(surface))
    {
        assert(0 && "Out of memory");
    }

    float timer     = 0.0f;
    float timeStep  = 1.0f / 60.0f;

    int   fpsCount    = 0;
    int   fpsValue    = 0;
    float fpsTimer    = 0.0f;
    float fpsInterval = 1.0f;

    while (!WindowShouldClose())
    {
        timer += GetFrameTime();
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

        fpsTimer += GetFrameTime();
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

bool IsLiquidSurface2DValid(LiquidSurface2D surface)
{
    return !(surface.cols <= 0 || surface.rows <= 0 || !surface.points || !surface.fixedPoints || !surface.springs);
}

LiquidPoint2D NewLiquidPoint2D(Vector2 position, float invMass)
{
    return (LiquidPoint2D) {
        position, (Vector2) { 0, 0 }, (Vector2) { 0, 0 }, invMass, DEFAULT_POINT_DAMPING
    };
}

LiquidSpring2D NewLiquidSpring2D(LiquidPoint2D* p0, LiquidPoint2D* p1, float stiffness, float damping)
{
    return (LiquidSpring2D) {
        p0, p1,

        Vector2Distance(p0->position, p1->position) * 0.95f,
        stiffness, damping, 250.0f
    };
}

LiquidSurface2D NewLiquidSurface2D(Rectangle bounds, Vector2 spacing)
{
    int cols = (int)(bounds.width / spacing.x) + 2;
    int rows = (int)(bounds.height / spacing.y) + 2;

    int pointCount = cols * rows;
    Array(LiquidPoint2D) points = ArrayNew(LiquidPoint2D, pointCount);
    Array(LiquidPoint2D) fixedPoints = ArrayNew(LiquidPoint2D, pointCount);
    Array(LiquidSpring2D) springs = ArrayNew(LiquidSpring2D, 4 * pointCount);

    if (!points || !fixedPoints || !springs)
    {
        ArrayFree(springs);
        ArrayFree(fixedPoints);
        ArrayFree(points);

        return (LiquidSurface2D) { 0 };
    }

    ArraySetCount(points, pointCount);
    ArraySetCount(fixedPoints, pointCount);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int index = i * cols + j;

            Vector2 position = { bounds.x + j * spacing.x, bounds.y + i * spacing.y };
            points[index] = NewLiquidPoint2D(position, 1.0f);
            fixedPoints[index] = NewLiquidPoint2D(position, 1.0f);

            if (i == 0 || j == 0 || i == rows - 1 || j == cols - 1)
            {
                ArrayPush(springs, NewLiquidSpring2D(&fixedPoints[index], &points[index], 0.2f, 5.0f));
            }
            else if (i % 3 == 0 && j % 3 == 0)
            {
                ArrayPush(springs, NewLiquidSpring2D(&fixedPoints[index], &points[index], 0.004f, 20.0f));
            }

            const float stiffness = 0.28f;
            const float damping = 2.0f;
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

        Vector2 diff = Vector2Subtract(spring.p0->position, spring.p1->position);
        float len = Vector2Length(diff);
        if (len > spring.targetLength)
        {
            float changeRate = (len - spring.targetLength) / len;
            Vector2 diffVelocity = Vector2Subtract(spring.p1->velocity, spring.p0->velocity);
            Vector2 force = Vector2Subtract(Vector2Scale(diff, spring.stiffness * changeRate), Vector2Scale(diffVelocity, fmaxf(0.0f, 1.0f - spring.damping * timeStep)));
            
            spring.p0->acceleration = Vector2Add(spring.p0->acceleration, Vector2Scale(Vector2Negate(force), spring.force * spring.p0->invMass * timeStep));
            spring.p1->acceleration = Vector2Add(spring.p1->acceleration, Vector2Scale(force, spring.force * spring.p1->invMass * timeStep));
        }
    }

    for (int i = 0, n = ArrayCount(surface.points); i < n; i++)
    {
        LiquidPoint2D point = surface.points[i];

        point.velocity = Vector2Add(point.velocity, Vector2Scale(point.acceleration, timeStep));
        point.position = Vector2Add(point.position, Vector2Scale(point.velocity, timeStep));

        point.acceleration = Vector2Scale(point.acceleration, fmaxf(0.0f, 1.0f - point.damping * timeStep));
        point.velocity = Vector2Scale(point.velocity, fmaxf(0.0f, 1.0f - point.damping * timeStep));

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
            Vector2 p0 = surface.points[i * cols + j].position;
            Vector2 p1 = surface.points[(i - 1) * cols + j].position;
            Vector2 p2 = surface.points[i * cols + (j - 1)].position;

            DrawLineEx(p0, p1, 1.0f, lineColor);
            DrawLineEx(p0, p2, 1.0f, lineColor);
        }
    }
}

void ApplyForceOnLiquidSurface2D(LiquidSurface2D surface, float force, Vector2 position, float radius, float timeStep)
{
    for (int i = 0, n = ArrayCount(surface.points); i < n; i++)
    {
        LiquidPoint2D point = surface.points[i];
        Vector2 diff = Vector2Subtract(point.position, position);
        float dist = Vector2Length(diff);
        if (dist < radius)
        {
            float accuratedForce = force / (100.0f + dist);
            Vector2 directionForce = Vector2Scale(diff, accuratedForce);

            point.acceleration = Vector2Add(point.acceleration, Vector2Scale(directionForce, point.invMass * timeStep));
            point.damping = point.damping * (1.0f / 0.6f);
            
            surface.points[i] = point;
        }
    }
}
