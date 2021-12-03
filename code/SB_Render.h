#ifndef SB_RENDER_H
#define SB_RENDER_H

#include "SB_Application.h"
#include "SB_Math.h"

struct Entity;
struct Camera
{
    GeneralizedPosition tilePosition;
    v2                  absolutePosition; // in meters
    Entity*             followingEntity;
    rect                screen;
};

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

void drawRectangle( RenderBuffer* buffer, v2 min, v2 max, Color c );
void drawImage( RenderBuffer* buffer, v2 position, Image* img );

#endif//SB_RENDER_H