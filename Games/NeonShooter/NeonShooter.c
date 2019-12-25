#include <MaiLib.h>
#include <MaiMath.h>

#include "NeonShooter_ParticleSystem.h"
#include "NeonShooter_World.h"

int main(void)
{
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Neon shooter");

    SetTargetFPS(60);

    InitParticles();
    World world = WorldNew();
    vec2 aim;
    bool fire;

    while (!PollWindowEvents())
    {
        float axis_vertical = 0.0f;
        float axis_horizontal = 0.0f;

        const float LERP_RATE = 0.5f;

        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        {
            axis_vertical = lerpf(axis_vertical, -1.0f, LERP_RATE);
        }
        else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        {
            axis_vertical = lerpf(axis_vertical, 1.0f, LERP_RATE);
        }
        else
        {
            axis_vertical = lerpf(axis_vertical, 0.0f, LERP_RATE);
        }

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            axis_horizontal = lerpf(axis_horizontal, -1.0f, LERP_RATE);
        }
        else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_R))
        {
            axis_horizontal = lerpf(axis_horizontal, 1.0f, LERP_RATE);
        }
        else
        {
            axis_horizontal = lerpf(axis_horizontal, 0.0f, LERP_RATE);
        }

        float mx = GetMouseX();
        float my = GetMouseY();
        bool fire = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        {
            vec2 clip = vec2From(2.0f * mx / GetScreenWidth() - 1.0f, 2.0f * my / GetScreenHeight() - 1.0f);

            vec2 mpos = vec2From(clip.x * GetScreenWidth(), clip.y * GetScreenHeight());

            vec2 taim = vec2Normalize(vec2Sub(mpos, world.player.position));

            aim = vec2Lerp(aim, taim, 0.8f);
        }

        if (IsGamepadAvailable(0))
        {
            float axis_left_x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
            float axis_left_y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
            axis_vertical = lerpf(axis_vertical, fabsf(axis_left_y) > 0.05f ? axis_left_y : 0.0f, LERP_RATE);
            axis_horizontal = lerpf(axis_horizontal, fabsf(axis_left_x) > 0.05f ? axis_left_x : 0.0f, LERP_RATE);

            float axis_right_x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
            float axis_right_y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
            float x = fabsf(axis_right_x) > 0.05f ? axis_right_x : 0.0f;
            float y = fabsf(axis_right_y) > 0.05f ? axis_right_y : 0.0f;
            if (vec2Length(vec2From(x, y)) < 0.01f)
            {
                aim = vec2Zero();
            }
            else
            {
                fire = true;


                float cur_angle = atan2f(aim.y, aim.x);
                float aim_angle = atan2f(y, x);

                cur_angle = lerpf(cur_angle, aim_angle, 0.8f);
                aim = vec2From(cosf(cur_angle), sinf(cur_angle));


                aim.x = lerpf(aim.x, x, 0.6f);
                aim.y = lerpf(aim.y, y, 0.6f);
            }
        }

        vec2 axes = vec2From(axis_horizontal, axis_vertical);
        if (vec2Length(axes) < 0.01f)
        {
            axes = vec2Zero();
        }
        else
        {
            float len = clampf(vec2Length(axes), 0, 1);
            float angle = atan2f(axes.y, axes.x);

            axes = vec2From(cosf(angle) * len, sinf(angle) * len);
        }

        axis_vertical = axes.y;
        axis_horizontal = axes.x;

        WorldUpdate(&world, axis_horizontal, axis_vertical, aim, fire, GetDeltaTime());
        UpdateParticles(&world, GetDeltaTime());

        BeginDrawing();
        {
            ClearBackground(BLACK);
            
            Camera2D camera = {
                (vec2) {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f},
                vec2Zero(),
                0,
                0.5f,
            };
            BeginMode2D(camera);
            {
                WorldRender(world);
                DrawParticles();
            }
            EndMode2D();

            DrawFPS(0, 0);
        }
        EndDrawing();
    }

    ReleaseParticles();
    WorldFree(&world);
    CloseWindow();
    return 0;
}