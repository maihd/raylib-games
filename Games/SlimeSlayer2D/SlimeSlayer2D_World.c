#include <MaiLib.h>
#include <MaiArray.h>

#include "SlimeSlayer2D_World.h"

World WorldNew(TileSheet tileSheet, int tileCols, int tileRows, float tileWidth, float tileHeight, int groundHeight, ivec2 backgroundTile, ivec2 upperGroundTile, ivec2 lowerGroundTite)
{
    World world = { 0 };

    world.tileSheet     = tileSheet;
    world.tileCols      = tileCols;
    world.tileRows      = tileRows;
    world.tileWidth     = tileWidth;
    world.tileHeight    = tileHeight;

    world.groundHeight      = groundHeight;
    world.backgroundTile    = backgroundTile;
    world.lowerGroundTite   = lowerGroundTite;
    world.upperGroundTile   = upperGroundTile;

    return world;
}

void WorldFree(World world)
{
    // NOOP
}

void WorldUpdate(World* world, float timeStep)
{
    // NOOP
}

void WorldRender(World world)
{
    int screenWidth = world.tileCols * world.tileWidth;
    int screenHeight = world.tileRows * world.tileHeight;

    ivec2 backgroundTile = world.backgroundTile;
    ivec2 lowerGroundTite = world.lowerGroundTite;
    ivec2 upperGroundTile = world.upperGroundTile;

    for (int i = 0; i < world.tileRows; i++)
    {
        for (int j = 0; j < world.tileCols; j++)
        {
            int x = j * world.tileWidth;
            int y = i * world.tileHeight;

            DrawTile(world.tileSheet, backgroundTile.x, backgroundTile.y, vec2New((float)x, (float)y), vec2New(0, 0), false, WHITE);
        }
    }

    for (int i = 0; i <= world.tileCols; i++)
    {
        int x = i * world.tileWidth;

        DrawTile(world.tileSheet, upperGroundTile.x, upperGroundTile.y, vec2New(x, screenHeight - (world.groundHeight - 1) * world.tileWidth), vec2New(0, 1.0f), false, WHITE);
        for (int j = 0; j < world.groundHeight - 1; j++)
        {
            DrawTile(world.tileSheet, lowerGroundTite.x, lowerGroundTite.y, vec2New(x, screenHeight - j * world.tileWidth), vec2New(0, 1.0f), false, WHITE);
        }
    }
}
