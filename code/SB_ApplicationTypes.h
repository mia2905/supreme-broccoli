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
    f32   x;
    f32   y;
    f32   width;
    f32   height;
    Color color;
};

struct ApplicationState
{
    Player player;
};

#endif//SB_APPLICATIONTYPES_H