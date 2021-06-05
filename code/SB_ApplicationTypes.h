#ifndef SB_APPLICATIONTYPES_H
#define SB_APPLICATIONTYPES_H

#include "SB_Application.h"

struct Pixel
{
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

struct Color
{
    f32 red;
    f32 green;
    f32 blue;
    f32 alpha;
};

struct RawPosition
{
    u32 tileMapX;
    u32 tileMapY;

    f32 x; // position x relative to a tilemap
    f32 y; // position y relative to a tilemap
};

struct CorrectPosition
{
    u32 tileMapX;
    u32 tileMapY;

    u32 tileX;
    u32 tileY;

    f32 x; // position x relative to a tile
    f32 y; // position y relative to a tile
}; 

struct Player
{
    f32   x;
    f32   y;
    f32   width;
    f32   height;
    f32   speed; // pixels per second;
    Color color;
};

struct TileMap
{
    u32* tiles;
};

struct World
{
    u32 tilemapCountX; // number of maps in X
    u32 tilemapCountY; // number of maps in Y

    u32 tileCountX;    // number of tiles per map in X
    u32 tileCountY;    // number of tiles per map in Y

    u32 tileWidth;
    u32 tileHeight;

    u32 tilemapX;      // current map index in x
    u32 tilemapY;      // current map index in y

    TileMap tilemaps[4];
};

struct ApplicationState
{
    bool    reload;
    Player  player;
    World   world;
};

#endif//SB_APPLICATIONTYPES_H