#include "SB_Render.h"
#include "SB_Math.h"

void drawRectangle( RenderBuffer* buffer, v2 min, v2 max, Color c )
{
    u32 screenWidth  = buffer->width;
    u32 screenHeight = buffer->height;

    s32 xmin = roundToS32( min.x );
    s32 ymin = roundToS32( min.y );
    s32 xmax = roundToS32( max.x );
    s32 ymax = roundToS32( max.y );

    if( xmin < 0 ) xmin = 0;
    if( xmax < 0 ) xmax = 0;
    if( xmin >= screenWidth ) return; // offscreen
    if( xmax >= screenWidth ) xmax = screenWidth;

    if( ymin < 0 ) ymin = 0;
    if( ymax < 0 ) ymax = 0;
    if( ymin >= screenHeight ) return; // offscreen
    if( ymax >= screenHeight ) ymax = screenHeight;

    // the start of the rectangle is the pixel at minX / minY
    for( u32 row=ymin; row<ymax; ++row )
    {
        u8* base = buffer->buffer + (u32)((row * buffer->width + xmin) * sizeof(Pixel));
        Pixel* p = (Pixel*)base;
        for( u32 col=xmin; col<xmax; ++col )
        {
            p->red   = (u8)(c.red   * 255);
            p->green = (u8)(c.green * 255);
            p->blue  = (u8)(c.blue  * 255);
            p->alpha = (u8)(c.alpha * 255);
            ++p;
        }
    }
}

void drawImage( RenderBuffer* buffer, v2 position, Image* img )
{
    u32 screenWidth  = buffer->width;
    u32 screenHeight = buffer->height;
    u32 imgWidth     = img->width;
    u32 imgHeight    = img->height;
    u32 srcRow       = 0;
    u32 srcCol       = 0;

    s32 xmin = roundToS32( position.x );
    s32 ymin = roundToS32( position.y );
    s32 xmax = roundToS32( position.x + img->width );
    s32 ymax = roundToS32( position.y + img->height );

    if( xmin < 0 ) 
    {
        srcCol = -xmin;
        xmin   = 0;
    }

    if( ymin < 0 ) 
    {
        srcRow = -ymin;
        ymin = 0;
    }

    if( xmin >= screenWidth ) return; // offscreen
    if( ymin >= screenHeight ) return; // offscreen

    if( xmax >= screenWidth )  xmax = screenWidth  - 1;
    if( ymax >= screenHeight ) ymax = screenHeight - 1;

    // the start of the rectangle is the pixel at minX / minY
    for( u32 row=ymin; row<ymax; ++row )
    {
        u8* base = buffer->buffer + (u32)((row * buffer->width + xmin) * sizeof(Pixel));
        u8* src  = img->data + (u32)((srcRow * imgWidth + srcCol) * sizeof(Pixel)); 
        Pixel* dest = (Pixel*)base;
        for( u32 col=xmin; col<xmax; ++col )
        {
            f32 red   = (f32)src[0] / 255.0f;
            f32 green = (f32)src[1] / 255.0f;
            f32 blue  = (f32)src[2] / 255.0f;
            f32 alpha = (f32)src[3] / 255.0f;

            f32 destRed   = (f32)dest->red   / 255.0f;
            f32 destGreen = (f32)dest->green / 255.0f;
            f32 destBlue  = (f32)dest->blue  / 255.0f;
            f32 destAlpha = (f32)dest->alpha / 255.0f;

            // dest = src * alpha + dest( 1 - alpha )
            dest->red   = (u8)((red   * alpha + destRed   * (1.0f - alpha)) * 255.0f);
            dest->green = (u8)((green * alpha + destGreen * (1.0f - alpha)) * 255.0f);
            dest->blue  = (u8)((blue  * alpha + destBlue  * (1.0f - alpha)) * 255.0f);
            dest->alpha = (u8)((alpha * alpha + destAlpha * (1.0f - alpha)) * 255.0f);
            ++dest;
            src += 4;
        }
        srcRow++;
    }
}


