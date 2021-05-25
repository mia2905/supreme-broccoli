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

    u32 xmin = RoundToU32( minX );
    u32 ymin = RoundToU32( minY );
    u32 xmax = RoundToU32( maxX );
    u32 ymax = RoundToU32( maxY );

    if( xmin < 0 ) xmin = 0;
    if( xmax < 0 ) xmax = 0;
    if( xmin >= screenWidth ) return; // offscreen
    if( xmax >= screenWidth ) xmax = screenWidth;

    if( ymin < 0 ) ymin = 0;
    if( ymax < 0 ) ymax = 0;
    if( ymin >= screenHeight ) return; // offscreen
    if( ymax >= screenHeight ) maxY = screenHeight;

    u32 width  = xmax - xmin;
    u32 height = ymax - ymin;

    // the start of the rectangle is the pixel at minX / minY
    for( u32 row=ymin; row<=ymax; ++row )
    {
        u8* base = buffer->buffer + (u32)((row * buffer->width + xmin) * sizeof(Pixel));
        Pixel* p = (Pixel*)base;
        for( u32 col=xmin; col<xmax; ++col )
        {
            p->red   = (u8)(c.red * 255);
            p->green = (u8)(c.green * 255);
            p->blue  = (u8)(c.blue * 255);
            p->alpha = (u8)(c.alpha * 255);
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