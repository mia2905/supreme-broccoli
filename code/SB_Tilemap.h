#ifndef SB_TILEMAP_H
#define SB_TILEMAP_H

#include "SB_Application.h"

#define TILEAREA_SHIFT 8
#define TILE_MASK      0x000000ff

struct GeneralizedPosition
{
    u32 unifiedPositionX; // upper 24-bits tilearea and lower 8-bits tile inside the tile area
    u32 unifiedPositionY; // upper 24-bits tilearea and lower 8-bits tile inside the tile area

    v2 tileRelative;      // in meters from the top left corner of the tile
};

struct DecomposedPosition
{
    s32 tileareaX;
    s32 tileareaY;

    s32 tileX;
    s32 tileY;

    v2 tileRelative;
};

struct Tile
{
    u32 x;
    u32 y;
    u32 areaX;
    u32 areaY;

    Tile( u32 x, u32 y, u32 areaX, u32 areaY )
    {
        this->x = x;
        this->y = y;
        this->areaX = areaX;
        this->areaY = areaY;
    }
};

enum DOOR_DIRECTION
{
    NONE   = 0,
    LEFT   = 1,
    RIGHT  = 2,
    TOP    = 3,
    BOTTOM = 4
};

struct TileArea
{
    u32* tiles;
};

struct TileMap
{
    u32 tileCountX;    // number of tiles per map in X
    u32 tileCountY;    // number of tiles per map in Y

    u32 tileInPixels;
    f32 tileInMeters;
    f32 metersToPixels;

    TileArea* tileAreas;
    Image*    brickImage;
};

void printPosition( GeneralizedPosition p, TileMap* tilemap );
DecomposedPosition decomposePosition( GeneralizedPosition p );

#endif//SB_TILEMAP_H