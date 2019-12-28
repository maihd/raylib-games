#include <MaiLib.h>

#include "SlimeSlayer2D_Assets.h"

const char* GetAssetsPath(const char* targetPath)
{
#ifdef RELEASE
    const char* ASSET_PATH = "Assets";
#else
    const char* ASSET_PATH = "../Binary/SlimeSlayer2D/Assets";
#endif

    return TextFormat("%s/%s", ASSET_PATH, targetPath);
}
