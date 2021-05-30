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

struct Player
{
    u32   tilemapX;
    u32   tilemapY;
    f32   x;
    f32   y;
    f32   width;
    f32   height;
    f32   speed; // pixels per second;
    Color color;
};

struct TileMap
{
    u32  countX;    // number of tiles in x
    u32  countY;    // number of tiles in y
    u32  tileWidth;
    u32  tileHeight;
    u32* tiles;
};

struct World
{
    u32 tilemapCountX;
    u32 tilemapCountY;

    u32 tilemapX;
    u32 tilemapY;

    TileMap tilemaps[4];
};

struct ApplicationState
{
    Player  player;
    World   world;
};

#endif//SB_APPLICATIONTYPES_H