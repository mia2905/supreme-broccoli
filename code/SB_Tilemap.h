#ifndef SB_TILEMAP_H
#define SB_TILEMAP_H

#include "SB_Application.h"
#include "SB_Math.h"

#define NR_OF_TILEAREAS     100
#define TILEAREA_SHIFT      8
#define TILE_MASK           0x000000ff
#define TILE_SIZE           40
#define TILES_PER_AREA_Y    WINDOW_HEIGHT / TILE_SIZE
#define TILES_PER_AREA_X    WINDOW_WIDTH  / TILE_SIZE

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

    TileArea* tileAreas;
};

TileArea*           getTileArea( TileMap* tilemap, s32 x, s32 y );
u32                 getTileValue( TileMap* tilemap, TileArea* area, u32 tileX, u32 tileY );

DecomposedPosition  decomposePosition( GeneralizedPosition p );
GeneralizedPosition composePosition( DecomposedPosition pos );
GeneralizedPosition updatePosition( GeneralizedPosition p, v2 movement, TileMap* tilemap );
GeneralizedPosition buildNewPosition( DecomposedPosition old, v2 movement, TileMap* tilemap );
GeneralizedPosition buildPosition( u32 tileX, u32 tileY, u32 tileAreaX, u32 tileAreaY, v2 tileRelative );


s32 buildNewTileCoord( s32 tile, s32 tileCount );
s32 generalizeTileIndex( s32* tileIndex, s32 tileCount );

TileArea* buildTileArea( MemoryPool* pool, TileMap* map, u32 areaX, u32 areaY );


#endif//SB_TILEMAP_H