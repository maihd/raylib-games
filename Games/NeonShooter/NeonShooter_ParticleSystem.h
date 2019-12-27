#pragma once

#include <MaiDef.h>
#include "NeonShooter_World.h"

void InitParticles(void);
void ClearParticles(void);
void ReleaseParticles(void);

void SpawnParticle(Texture texture, vec2 position, vec4 color, float duration, vec2 scale, float theta, vec2 velocity);

void UpdateParticles(World* world, float dt);
void DrawParticles(void);