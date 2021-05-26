#include "SB_Application.h"
#include "SB_ApplicationTypes.h"

#include <stdio.h>

s32 RoundToU32( f32 value )
{
    return (u32)(value + 0.5f);
}

void drawRectangle( RenderBuffer* buffer, f32 minX, f32 minY, f32 maxX, f32 maxY, Color c )
{
    u32 screenWidth  = buffer->width;
    u32 screenHeight = buffer->height;

    s32 xmin = RoundToU32( minX );
    s32 ymin = RoundToU32( minY );
    s32 xmax = RoundToU32( maxX );
    s32 ymax = RoundToU32( maxY );

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

void drawPlayer( RenderBuffer* buffer, Player* player )
{
    f32 minx = player->x - player->width/2;
    f32 maxx = player->x + player->width/2;
    f32 miny = player->y - player->height/2;
    f32 maxy = player->y + player->height/2;
    drawRectangle( buffer, minx, miny, maxx, maxy, player->color );
}

void updatePlayer( UserInput* input, Player* player )
{
    f32 speed = 10.0f;
    if( input->arrowUp.endedDown )    player->y -= speed;
    if( input->arrowDown.endedDown )  player->y += speed;
    if( input->arrowRight.endedDown ) player->x += speed;
    if( input->arrowLeft.endedDown )  player->x -= speed;
}

void UpdateAndRender( ApplicationMemory* memory, RenderBuffer* buffer, UserInput* input )
{
    ApplicationState* state  = (ApplicationState*)memory->permanentMemory;
    Player*           player = &state->player;

    if( !memory->isInitialized )
    {
        player->x      = 30;
        player->y      = 30;
        player->width  = 40;
        player->height = 40;
        player->color  = { 1.0, 1.0, 0.0, 1.0 };
        memory->isInitialized = true;
    }

    player->color = { 0.0, 0.0, 1.0, 1.0 };

    Color background = { 0.0f, 0.8f, 0.5f, 1.0f };
    drawRectangle( buffer, 0, 0, buffer->width, buffer->height, background );

    updatePlayer( input, player );
    drawPlayer( buffer, player );
}