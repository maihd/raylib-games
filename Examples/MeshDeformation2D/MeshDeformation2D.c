#include <MaiLib.h>
#include <MaiMath.h>
#include <MaiArray.h>

typedef struct DeformMesh2D
{
    int width;
    int height;

    float cellWidth;
    float cellHeight;

    float damping;
    float springForce;

    Array(vec2) velocities;
    Array(vec2) originVertices;
    Array(vec2) displacedVertices;
} DeformMesh2D;

DeformMesh2D    NewDeformMesh2D(rect bounds, vec2 spacing, float springForce, float damping);
void            FreeDeformMesh2D(DeformMesh2D mesh);

void            UpdateDeformMesh2D(DeformMesh2D mesh, float dt);
void            RenderDeformMesh2D(DeformMesh2D mesh, Color lineColor);

void            ApplyForceOnDeformMesh2D(DeformMesh2D mesh, float force, vec2 position, float dt);

int main(void)
{
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mesh Deformation 2D");
    SetTargetFPS(60);

    DeformMesh2D mesh = NewDeformMesh2D((rect){ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, vec2New(64, 64), 20.0f, 5.0f);

    while (!WindowShouldClose())
    {
        UpdateDeformMesh2D(mesh, GetDeltaTime());

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            ApplyForceOnDeformMesh2D(mesh, 100.0f, GetMousePosition(), GetDeltaTime());
        }

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            RenderDeformMesh2D(mesh, DARKGRAY);

            DrawFPS(0, 0);
        }
        EndDrawing();
    }

    FreeDeformMesh2D(mesh);

    CloseWindow();
    return 0;
}

DeformMesh2D NewDeformMesh2D(rect bounds, vec2 spacing, float springForce, float damping)
{
    int cols = (int)(bounds.width / spacing.x) + 2;
    int rows = (int)(bounds.height / spacing.y) + 2;

    int vertexCount = cols * rows;
    Array(vec2) velocities = ArrayNew(vec2, vertexCount);
    Array(vec2) originVertices = ArrayNew(vec2, vertexCount);
    Array(vec2) displacedVertices = ArrayNew(vec2, vertexCount);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int index = i * cols + j;
            vec2 vertex = vec2New(bounds.x + j * spacing.x, bounds.y + i * spacing.y);

            velocities[index] = vec2New(0, 0);
            originVertices[index] = vertex;
            displacedVertices[index] = vertex;
        }
    }

    return (DeformMesh2D) {
        .width = cols,
        .height = rows,

        .cellWidth = spacing.x,
        .cellHeight = spacing.y,

        .damping = damping,
        .springForce = springForce,

        .velocities = velocities,
        .originVertices = originVertices,
        .displacedVertices = displacedVertices,
    };
}

void FreeDeformMesh2D(DeformMesh2D mesh)
{
    ArrayFree(mesh.velocities);
    ArrayFree(mesh.originVertices);
    ArrayFree(mesh.displacedVertices);
}

void UpdateDeformMesh2D(DeformMesh2D mesh, float dt)
{
    Array(vec2) velocities = mesh.velocities;
    Array(vec2) originVertices = mesh.originVertices;
    Array(vec2) displacedVertices = mesh.displacedVertices;

    for (int i = 0, n = mesh.width * mesh.height; i < n; i++)
    {
        vec2 point = displacedVertices[i];
        vec2 displacement = vec2Sub(point, originVertices[i]);
        vec2 velocity = vec2Scale(vec2Sub(velocities[i], vec2Scale(displacement, mesh.springForce * dt)), fmaxf(0.0f, 1.0f - mesh.damping * dt));
        
        velocities[i] = velocity;
        displacedVertices[i] = vec2Add(point, vec2Scale(velocity, dt));
    }
}

void RenderDeformMesh2D(DeformMesh2D mesh, Color lineColor)
{
    int cols = mesh.width;
    int rows = mesh.height;

    for (int i = 1; i < rows; i++)
    {
        for (int j = 1; j < cols; j++)
        {
            vec2 p0 = mesh.displacedVertices[i * cols + j];
            vec2 p1 = mesh.displacedVertices[(i - 1) * cols + j];
            vec2 p2 = mesh.displacedVertices[i * cols + (j - 1)];
            vec2 p3 = mesh.displacedVertices[(i - 1) * cols + (j - 1)];

#if 0
            DrawLineEx(vec2Scale(vec2Add(p3, p1), 0.5f), vec2Scale(vec2Add(p2, p0), 0.5f), 1.0f, lineColor);
            DrawLineEx(vec2Scale(vec2Add(p3, p2), 0.5f), vec2Scale(vec2Add(p1, p0), 0.5f), 1.0f, lineColor);
#endif

#if 0
            if (j > 1)
            {
                float thickness = j % 3 == 1 ? 2.0f : 1.0f;

                // use Catmull-Rom interpolation to help smooth bends in the grid
                int clampedX = (int)(fmaxf(j + 1, cols - 1));
                vec2 mid = vec2CatmullRom(mesh.displacedVertices[i * cols + (j - 2)], p2, p0, mesh.displacedVertices[i * cols + clampedX], 0.5f);

                // If the grid is very straight here, draw a single straight line. Otherwise, draw lines to our
                // new interpolated midpoint
                if (vec2DistanceSq(mid, vec2Scale(vec2Add(p2, p0), 0.5f)) > 1.0f)
                {
                    DrawLineEx(p2, mid, thickness, lineColor);
                    DrawLineEx(p0, mid, thickness, lineColor);
                }
                else
                {
                    DrawLineEx(p2, p0, thickness, lineColor);
                }
            }
            else
#endif
            {
                float thickness = 1.0f;
                DrawLineEx(p0, p2, thickness, lineColor);
            }
            
            //if (i > 10000)
            //{
            //    float thickness = i % 3 == 1 ? 2.0f : 1.0f;
            //
            //    // use Catmull-Rom interpolation to help smooth bends in the grid
            //    int clampedY = (int)(fmaxf(i + 1, rows - 1));
            //    vec2 mid = vec2CatmullRom(mesh.displacedVertices[(i - 2) * cols + j], p1, p0, mesh.displacedVertices[clampedY * cols + j], 0.5f);
            //
            //    // If the grid is very straight here, draw a single straight line. Otherwise, draw lines to our
            //    // new interpolated midpoint
            //    if (vec2DistanceSq(mid, vec2Scale(vec2Add(p1, p0), 0.5f)) > 1.0f)
            //    {
            //        DrawLineEx(p1, mid, thickness, lineColor);
            //        DrawLineEx(p0, mid, thickness, lineColor);
            //    }
            //    else
            //    {
            //        DrawLineEx(p1, p0, thickness, lineColor);
            //    }
            //}
            //else
            {
                float thickness = 1.0f;
                DrawLineEx(p0, p1, thickness, lineColor);
            }
            
        }
    }
}

void ApplyForceOnDeformMesh2D(DeformMesh2D mesh, float force, vec2 position, float dt)
{
    for (int i = 0, n = mesh.width * mesh.height; i < n; i++)
    {
        vec2 point = mesh.displacedVertices[i];
        vec2 pointToVertex = vec2Sub(point, position);
        
        float accuratedForce = force / (1.0f + vec2Length(pointToVertex));

        mesh.velocities[i] = vec2Add(mesh.velocities[i], vec2Scale(vec2Normalize(pointToVertex), accuratedForce * dt));
    }
}
