#include "NeonShooter_Assets.h"

#include <MaiLib.h>
#include <MaiArray.h>

typedef struct
{
    u64     hash;
    Texture texture;
} CachedTexture;

static u64 HashString(const char* path)
{
    u64 h = 0;
    int length = (int)strlen(path);
    const u8* key = (const u8*)path;

    if (length > 3)
    {
        const u32* key_x4 = (const u32*)key;
        int i = length >> 2;

        do
        {
            u32 k = *key_x4++;
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;
            h ^= k;
            h = (h << 13) | (h >> 19);
            h = (h * 5) + 0xe6546b64;
        } while (--i);

        key = (const u8*)key_x4;
    }

    if (length & 3)
    {
        int i = length & 3;
        u32 k = 0;
        key = &key[i - 1];

        do {
            k <<= 8;
            k |= *key--;
        } while (--i);

        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k;
    }

    h ^= length;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

static Array(CachedTexture) cachedTextures;

void    InitCacheTextures(void)
{
    cachedTextures = ArrayNew(CachedTexture, 32);
}

void    ClearCacheTextures(void)
{
    for (int i = 0, n = ArrayCount(cachedTextures); i < n; i++)
    {
        UnloadTexture(cachedTextures[i].texture);
    }

    ArrayFree(cachedTextures);
}

const char* GetAssetPath(const char* target)
{
#ifdef RELEASE
#   define ASSET_PATH "Assets"
#else
#   define ASSET_PATH "../Binary/NeonShooter/Assets"
#endif

    const char* finalPath = TextFormat("%s/%s", ASSET_PATH, target);
    return finalPath;
}

Texture CacheTexture(const char* path)
{
    const char* finalPath = GetAssetPath(path);
    u64 targetHash = HashString(finalPath);
    
    for (int i = 0, n = ArrayCount(cachedTextures); i < n; i++)
    {
        CachedTexture cachedTexture = cachedTextures[i];
        if (cachedTexture.hash == targetHash)
        {
            return cachedTexture.texture;
        }
    }

    Texture texture = LoadTexture(finalPath);
    if (texture.id != 0)
    {
        CachedTexture newEntry = { .hash = targetHash, .texture = texture };
        ArrayPush(cachedTextures, newEntry);
    }

    return texture;
}
