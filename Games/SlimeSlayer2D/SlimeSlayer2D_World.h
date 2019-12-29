#pragma once

#include <MaiDef.h>
#include <MaiFreeList.h>

#include "SlimeSlayer2D_TileSheet.h"

typedef struct World
{
    TileSheet   tileSheet;
    int         tileCols;
    int         tileRows;
    int         tileWidth;
    int         tileHeight;
    
    int         groundHeight;
    ivec2       backgroundTile;
    ivec2       upperGroundTile;
    ivec2       lowerGroundTite;
} World;

World WorldNew(TileSheet tileSheet, int tileCols, int tileRows, float tileWidth, float tileHeight, int groundHeight, ivec2 backgroundTile, ivec2 upperGroundTile, ivec2 lowerGroundTite);
void  WorldFree(World world);

void  WorldUpdate(World* world, float timeStep);
void  WorldRender(World world);
