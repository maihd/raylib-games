#include "NeonShooter_Assets.h"

#include <MaiLib.h>

u64 HashString(const char* path)
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

void    InitCacheTextures(void)
{

}

void    ClearCacheTextures(void)
{

}

Texture CacheTexture(const char* path)
{
#ifdef RELEASE
#   define ASSET_PATH "Assets"
#else
#   define ASSET_PATH "../Games/NeonShooter/Assets"
#endif

    return LoadTexture(TextFormat("%s/%s", ASSET_PATH, path));
}
