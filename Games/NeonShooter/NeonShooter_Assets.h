#pragma once

#include <MaiDef.h>

const char* GetAssetPath(const char* target);

void    InitCacheTextures(void);
void    ClearCacheTextures(void);

Texture CacheTexture(const char* path);