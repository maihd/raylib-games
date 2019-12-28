#include <MaiLib.h>

#include "SlimeSlayer2D_TileSheet.h"

TileSheet LoadTileSheet(const char* path, int cols, int rows)
{
    Texture texture = LoadTexture(path);
    if (texture.id)
    {
        return (TileSheet) {
            .cols = cols,
                .rows = rows,
                .texture = texture
        };
    }
    else
    {
        return (TileSheet) { 0 };
    }
}

TileSheet LoadTileSheetWithTileSize(const char* path, int tileWidth, int tileHeight)
{
    Texture texture = LoadTexture(path);
    if (texture.id)
    {
        return (TileSheet) {
            .cols = texture.width / tileWidth,
                .rows = texture.height / tileHeight,
                .texture = texture
        };
    }
    else
    {
        return (TileSheet) { 0 };
    }
}

void UnloadTileSheet(TileSheet sheet)
{
    UnloadTexture(sheet.texture);
}

void DrawTile(TileSheet sheet, int col, int row, vec2 position, vec2 pivot, bool flippedX, Color tint)
{
    float tileWidth = 1.0f / sheet.cols * sheet.texture.width;
    float tileHeight = 1.0f / sheet.rows * sheet.texture.height;

    rect tileRect = rectNew(col * tileWidth, row * tileHeight, (flippedX ? -1.0f : 1.0f) * tileWidth, tileHeight);
    rect drawRect = rectNew(position.x, position.y, tileWidth, tileHeight);

    DrawTexturePro(sheet.texture, tileRect, drawRect, vec2New(pivot.x * tileWidth, pivot.y * tileHeight), 0.0f, tint);
}