#ifndef SB_TILEMAP_H
#define SB_TILEMAP_H

#include "SB_Application.h"

#define TILEAREA_SHIFT 8
#define TILE_MASK      0x000000ff

#define TILEMAP_Y      256
#define TILEMAP_X      256
#define TILE_WIDTH     40

struct GeneralizedPosition
{
    u32 unifiedPositionX; // upper 24-bits tilearea and lower 8-bits tile inside the tile area
    u32 unifiedPositionY; // upper 24-bits tilearea and lower 8-bits tile inside the tile area

    f32 x; // position x relative to a tile
    f32 y; // position y relative to a tile
};

struct DecomposedPosition
{
    u32 tileareaX;
    u32 tileareaY;

    u32 tileX;
    u32 tileY;
};

struct TileArea
{
    u32* tiles;
};

struct TileMap
{
    u32 tileareaCountX; // number of maps in X
    u32 tileareaCountY; // number of maps in Y

    u32 tileCountX;    // number of tiles per map in X
    u32 tileCountY;    // number of tiles per map in Y

    u32 tileInPixels;
    f32 tileInMeters;
    f32 metersToPixels;

    TileArea* tilemaps;
};

#endif//SB_TILEMAP_H