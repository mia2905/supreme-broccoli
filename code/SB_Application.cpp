#include "SB_Application.h"
#include "SB_ApplicationTypes.h"

u32 RoundToU32( f32 value )
{
    return (u32)(value + 0.5f);
}

void drawRectangle( RenderBuffer* buffer, f32 minX, f32 minY, f32 maxX, f32 maxY, Color c )
{
    u32 screenWidth  = buffer->width;
    u32 screenHeight = buffer->height;

    if( minX < 0 ) minX = 0;
    if( maxX < 0 ) maxX = 0;
    if( minX >= screenWidth ) minX = screenWidth;
    if( maxX >= screenWidth ) maxX = screenWidth;

    if( minY < 0 ) minY = 0;
    if( maxY < 0 ) maxY = 0;
    if( minY >= screenHeight ) minY = screenHeight;
    if( maxY >= screenHeight ) maxY = screenHeight;

    u32 width  = maxX - minX;
    u32 height = maxY - minY;

    // the start of the rectangle is the pixel at minX / minY
    for( u32 row=minY; row<=maxY; ++row )
    {
        u8* base = buffer->buffer + (u32)((row * buffer->width + minX) * sizeof(Pixel));
        Pixel* p = (Pixel*)base;
        for( u32 col=minX; col<maxX; ++col )
        {
            p->red   = (u8)RoundToU32(c.red * 255);
            p->green = (u8)RoundToU32(c.green * 255);
            p->blue  = (u8)RoundToU32(c.blue * 255);
            p->alpha = (u8)RoundToU32(c.alpha * 255);
            ++p;
        }
    }
}

void UpdateAndRender( ApplicationMemory* memory, RenderBuffer* buffer, UserInput* input )
{
    if( !memory->isInitialized )
    {
        memory->isInitialized = true;
    }

    Color yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
    drawRectangle( buffer, 10, 10, 60, 60, yellow );
}