#pragma once

#include <raylib.h>
#include "NeonShooter_World.h"

void InitParticles(void);
void ClearParticles(void);
void ReleaseParticles(void);

void SpawnParticle(Texture texture, Vector2 position, Vector4 color, float duration, Vector2 scale, float theta, Vector2 velocity);

void UpdateParticles(World* world, float dt);
void DrawParticles(void);