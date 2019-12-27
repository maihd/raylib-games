#include <MaiLib.h>
#include <MaiMath.h>

#include <time.h>
#include <stdlib.h>

#include "NeonShooter_World.h"
#include "NeonShooter_Assets.h"
#include "NeonShooter_GameAudio.h"
#include "NeonShooter_ParticleSystem.h"

int main(void)
{
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    srand((u32)(time(0)));

    SetConfigFlags(FLAG_VSYNC_HINT);
    SetTargetFPS(60);
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Neon shooter");

    GameAudioInit();
    InitCacheTextures();
    InitParticles(); 

    World world = WorldNew();
    vec2 aim;
    bool fire;

#if 0
    const char* bloomShaderSource =
        "#version 330 core\n"
        
        "in vec2 fragTexCoord;"
        "in vec4 fragColor;"
        
        "out vec4 finalColor;"

        "uniform sampler2D image;"
        
        "void main() {"
            "float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);"
            
            "vec2 texSize = textureSize(image, 0);"
            "float size = max(texSize.x, texSize.y);"
            
            "vec2 texOffset = 1.0 / vec2(size, size);"
            "vec3 result = texture(image, fragTexCoord).rgb * weight[0];"
            
            "for(int i = 1; i < 5; ++i) {"
                "result += texture(image, fragTexCoord + vec2(texOffset.x * i, 0.0)).rgb * weight[i];"
                "result += texture(image, fragTexCoord - vec2(texOffset.x * i, 0.0)).rgb * weight[i];"
                "result += texture(image, fragTexCoord + vec2(0.0, texOffset.y * i)).rgb * weight[i];"
                "result += texture(image, fragTexCoord - vec2(0.0, texOffset.y * i)).rgb * weight[i];"
            "}"

            "result = texture(image, fragTexCoord).rgb + result;"
            "result = vec3(1.0) - exp(-result * 0.5f);"
            "result = pow(result, vec3(1.0 / 2.2));"
            "finalColor = vec4(result, 1.0);"
        "}";
#else
    const char* bloomShaderSource =
        "#version 330 core\n"

        // Input vertex attributes (from vertex shader)
        "in vec2 fragTexCoord;"
        "in vec4 fragColor;"

        // Output fragment color
        "out vec4 finalColor;"

        // Input uniform values
        "uniform sampler2D image;"
        "uniform vec4 colDiffuse;"

        "void main()"
        "{"
            "float samples = 5.0;"        // pixels per axis; higher = bigger glow, worse performance
            "float quality = 1.7;" 	      // lower = smaller glow, better quality

            "vec2 size = textureSize(image, 0);"
            "vec2 sizeFactor = vec2(1.0) / size * quality;"

            "vec4 sum = vec4(0);"

            // Texel color fetching from texture sampler
            "vec4 source = texture(image, fragTexCoord);"

            "int range = 2;"

            "for (int x = -range; x <= range; x++)"
            "{"
                "for (int y = -range; y <= range; y++)"
                "{"
                    "sum += texture(image, fragTexCoord + vec2(x, y) * sizeFactor);"
                "}"
            "}"

            // Calculate final fragment color
            "finalColor = ((sum / (samples * samples)) + source) * colDiffuse;"
        "}";
#endif

    Shader bloomShader = LoadShaderCode(0, bloomShaderSource);
    RenderTexture frame = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    float   timer    = 0.0f;
    float   timeStep = 1.0f / 60.0f;

    int     fpsCount    = 0;
    int     fpsValue    = 0;
    float   fpsTimer    = 0.0f;
    float   fpsInterval = 1.0f;

    while (!PollWindowEvents())
    {
        GameAudioUpdate();

        fpsTimer += GetDeltaTime();
        if (fpsTimer >= fpsInterval)
        {
            fpsTimer -= fpsInterval;

            fpsValue = (int)(fpsCount / fpsInterval);
            fpsCount = 0;
        }

        timer += GetDeltaTime();
        while (timer >= timeStep)
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
                vec2 clip = vec2New(2.0f * mx / GetScreenWidth() - 1.0f, 2.0f * my / GetScreenHeight() - 1.0f);

                vec2 mpos = vec2New(clip.x * GetScreenWidth(), clip.y * GetScreenHeight());

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
                float x = fabsf(axis_right_x) > 0.1f ? axis_right_x : 0.0f;
                float y = fabsf(axis_right_y) > 0.1f ? axis_right_y : 0.0f;
                if (vec2Length(vec2New(x, y)) < 0.01f)
                {
                    //aim = vec2New(0, 0);
                }
                else
                {
                    fire = true;


                    float cur_angle = atan2f(aim.y, aim.x);
                    float aim_angle = atan2f(y, x);

                    cur_angle = lerpf(cur_angle, aim_angle, 0.8f);
                    aim = vec2New(cosf(cur_angle), sinf(cur_angle));


                    aim.x = lerpf(aim.x, x, 0.6f);
                    aim.y = lerpf(aim.y, y, 0.6f);
                }
            }

            vec2 axes = vec2New(axis_horizontal, axis_vertical);
            if (vec2Length(axes) < 0.01f)
            {
                axes = vec2New(0, 0);
            }
            else
            {
                float len = clampf(vec2Length(axes), 0, 1);
                float angle = atan2f(axes.y, axes.x);

                axes = vec2New(cosf(angle) * len, sinf(angle) * len);
            }

            axis_vertical = axes.y;
            axis_horizontal = axes.x;

            fpsCount++;

            timer -= timeStep;

            WorldUpdate(&world, axis_horizontal, axis_vertical, aim, fire, timeStep);

            UpdateParticles(&world, timeStep);
        }

        BeginDrawing();
        {
            ClearBackground(BLACK);
            
            BeginTextureMode(frame);
            ClearBackground(BLACK);

            Camera2D camera = {
                (vec2) {GetScreenWidth() * 0.5f, GetScreenHeight() * 0.5f},
                vec2New(0, 0),
                0,
                0.5f,
            };
            BeginMode2D(camera);
            {
                WorldRender(world);
                DrawParticles();
            }
            EndMode2D();
            EndTextureMode();

            BeginShaderMode(bloomShader);
            DrawTextureRec(frame.texture, (rect) { 0, 0, SCREEN_WIDTH, -SCREEN_HEIGHT }, vec2New(0, 0), WHITE);
            EndShaderMode();

            DrawText(TextFormat("CPU FPS: %d", fpsValue), 0, 0, 18, RAYWHITE);
            DrawText(TextFormat("GPU FPS: %d", GetFPS()), 0, 24, 18, RAYWHITE);
        }
        EndDrawing();
    }

    WorldFree(&world);
    ReleaseParticles();

    GameAudioRelease();
    ClearCacheTextures();
    CloseWindow();
    return 0;
}