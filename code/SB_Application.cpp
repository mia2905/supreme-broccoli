#include "SB_Application.h"
#include "SB_ApplicationTypes.h"

#include <stdio.h>

#define TILEMAP_Y  13
#define TILEMAP_X  27
#define TILEWIDTH  40
#define TILEHEIGHT 40

static u32 tilemap[TILEMAP_Y][TILEMAP_X] = 
    {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1 },
        { 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1 },
        { 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1 },
        { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
    };

inline s32 roundToS32( f32 value )
{
    return (s32)(value + 0.5f);
}

void drawRectangle( RenderBuffer* buffer, f32 minX, f32 minY, f32 maxX, f32 maxY, Color c )
{
    u32 screenWidth  = buffer->width;
    u32 screenHeight = buffer->height;

    s32 xmin = roundToS32( minX );
    s32 ymin = roundToS32( minY );
    s32 xmax = roundToS32( maxX );
    s32 ymax = roundToS32( maxY );

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

void drawMap( RenderBuffer* buffer )
{
    Color tilecolor = { 0.4f, 0.4f, 0.4f, 1.0f };
    u32 tileWidth   = 40;
    u32 tileHeight  = 40;
    u32 tilesPerRow = 27;
    u32 rows        = 13;

    f32 minX = 0.0f;
    f32 maxX = 0.0f;
    f32 minY = 0.0f;
    f32 maxY = 0.0f;

    for( u32 row=0; row<rows; ++row )
    {
        for( u32 col=0; col<tilesPerRow; ++col )
        {
            u32 tile = tilemap[row][col];
            if( tile == 1 )
            {
                minX = col * tileWidth;
                minY = row * tileHeight;
                maxX = minX + tileWidth;
                maxY = minY + tileHeight;

                drawRectangle( buffer, minX, minY, maxX, maxY, tilecolor );
            }
        }
    }
}

bool checkCollision( f32 playerX, f32 playerY )
{
    bool collision = true;
    u32 playerTileX = (u32)( playerX / TILEWIDTH );
    u32 playerTileY = (u32)( playerY / TILEHEIGHT );

    if( (playerTileX >= 0) && (playerTileX < TILEMAP_X) &&
        (playerTileY >= 0) && (playerTileY < TILEMAP_Y) )
    {
       collision =  ( tilemap[playerTileY][playerTileX] == 0 ); 
    }

    return collision;
}

void updatePlayer( UserInput* input, Player* player )
{
    player->height = 40.0f;
    player->width  = 30.0f;
    player->color  = { 0.8f, 0.8f, 0.5f, 1.0f }; 
    f32 speed = 2.0f;

    f32 playerX = player->x;
    f32 playerY = player->y;

    if( input->arrowUp.isDown )    playerY -= speed;
    if( input->arrowDown.isDown )  playerY += speed;
    if( input->arrowRight.isDown ) playerX += speed;
    if( input->arrowLeft.isDown )  playerX -= speed;

    bool isValid = checkCollision( playerX, playerY );

    if( isValid )
    {
        player->x = playerX;
        player->y = playerY;
    }
}

void UpdateAndRender( ApplicationMemory* memory, RenderBuffer* buffer, UserInput* input )
{
    ApplicationState* state  = (ApplicationState*)memory->permanentMemory;
    Player*           player = &state->player;

    if( !memory->isInitialized )
    {
        player->x = 100;
        player->y = 100;

        memory->isInitialized = true;
    }

    player->color = { 0.0, 0.0, 1.0, 1.0 };

    Color background = { 0.9f, 0.2f, 0.8f, 1.0f };
    drawRectangle( buffer, 0, 0, buffer->width, buffer->height, background );

    drawMap( buffer );

    updatePlayer( input, player );
    drawPlayer( buffer, player );
}