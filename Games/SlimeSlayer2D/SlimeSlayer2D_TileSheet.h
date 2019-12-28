#pragma once

#include <MaiDef.h>

typedef struct TileSheet
{
    int cols;
    int rows;

    Texture texture;
} TileSheet;

TileSheet LoadTileSheet(const char* path, int cols, int rows);
TileSheet LoadTileSheetWithTileSize(const char* path, int tileWidth, int tileHeight);
void      UnloadTileSheet(TileSheet sheet);

void      DrawTile(TileSheet sheet, int col, int row, vec2 position, vec2 pivot, bool flippedX, Color tint);
