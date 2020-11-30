#pragma once

#include <raylib.h>

const char*     GetAssetPath(const char* target);

void            InitCacheTextures(void);
void            ClearCacheTextures(void);

Texture         CacheTexture(const char* path);
//const char*     CacheText(const char* path);
