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

struct GeneralizedPosition
{
    u32 tileMapX;
    u32 tileMapY;

    s32 tileX;
    s32 tileY;

    f32 x; // position x relative to a tile
    f32 y; // position y relative to a tile
}; 

struct Player
{
    GeneralizedPosition playerPos;
    f32   width;
    f32   height;
    f32   speed; // meters per second;
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

    u32 tileInPixels;
    f32 tileInMeters;
    f32 metersToPixels;

    TileMap tilemaps[4];
};

struct ApplicationState
{
    Player  player;
    World   world;
};

#endif//SB_APPLICATIONTYPES_H