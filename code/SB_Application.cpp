#include "SB_Application.h"
#include "SB_ApplicationTypes.h"

#include <stdio.h>
#include <math.h>

#define TILEMAPS_X  2
#define TILEMAPS_Y  2
#define TILEMAP_Y  13
#define TILEMAP_X  27
#define TILE_WIDTH 40

static u32 map_00[TILEMAP_Y][TILEMAP_X] = 
{
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1 },
    { 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1 },
    { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0 },
    { 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1 },
    { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1 },
    { 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1 },
    { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

static u32 map_01[TILEMAP_Y][TILEMAP_X] = 
{
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

static u32 map_10[TILEMAP_Y][TILEMAP_X] = 
{
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

static u32 map_11[TILEMAP_Y][TILEMAP_X] = 
{
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

inline s32 roundToS32( f32 value )
{
    return (s32)(value + 0.5f);
}

inline s32 truncateToS32( f32 value )
{
    return (s32)value;
}

inline s32 floorToS32( f32 value )
{
    return (s32)floorf( value );
}

void buildWorld( World* world )
{
    world->tilemaps[0].tiles = (u32*)map_00;
    world->tilemaps[1].tiles = (u32*)map_10;
    world->tilemaps[2].tiles = (u32*)map_01;
    world->tilemaps[3].tiles = (u32*)map_11;    
}

TileMap* getMap( World* world, u32 mapX, u32 mapY )
{
    u32 width    = world->tilemapCountX;
    TileMap* map = nullptr;

    if( (mapX >= 0) && (mapX < world->tilemapCountX) &&
        (mapY >= 0) && (mapY < world->tilemapCountY) )
    {
        map = &world->tilemaps[mapY * width + mapX];
    }
    
    return map;
}

TileMap* getCurrentMap( World* world, Player* player )
{
    u32 tilemapX = player->playerPos.tileMapX;
    u32 tilemapY = player->playerPos.tileMapY;
    u32 width    = world->tilemapCountX;
    TileMap* currentMap = nullptr;

    if( (tilemapX >= 0) && (tilemapX < world->tilemapCountX) &&
        (tilemapY >= 0) && (tilemapY < world->tilemapCountY) )
    {
        currentMap = &world->tilemaps[tilemapY * width + tilemapX];
    }
    
    return currentMap;
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

void drawPlayer( RenderBuffer* buffer, Player* player, World* world )
{
    f32 tileX = player->playerPos.tileX * world->tileWidth;
    f32 tileY = player->playerPos.tileY * world->tileHeight;

    f32 minx  = tileX + player->playerPos.x - player->width/2;
    f32 maxx  = tileX + player->playerPos.x + player->width/2;
    f32 miny  = tileY + player->playerPos.y - player->height/2;
    f32 maxy  = tileY + player->playerPos.y + player->height/2;
    drawRectangle( buffer, minx, miny, maxx, maxy, player->color );
}

void drawMap( RenderBuffer* buffer, World* world, Player* player )
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

    TileMap* currentMap = getCurrentMap( world, player );
    Assert( currentMap );

    for( u32 row=0; row<rows; ++row )
    {
        for( u32 col=0; col<tilesPerRow; ++col )
        {
            u32 tile = currentMap->tiles[row * world->tileCountX + col];
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

s32 generalizeCoords( f32* value, s32 pixelSize )
{
    s32 result = 0;
    f32 newX = *value / pixelSize;
    if( newX < 0 ) 
    {
        *value = pixelSize - *value;
        result = -1;
    }
    if( newX >= 1.0 )
    {
        *value = *value - pixelSize;
        result = 1;
    }

    return result;
}

s32 generalizeTileIndex( s32* tileIndex, s32 tileCount )
{
    s32 result = 0;
    s32 index = *tileIndex;

    if( index < 0 )
    {
        *tileIndex = tileCount + index;
        result = -1;
    }
    if( index >= tileCount )
    {
        *tileIndex = index - tileCount;
        result = 1;
    }

    return result;
}

GeneralizedPosition getGeneralizedPosition( World* world, GeneralizedPosition pos )
{
    GeneralizedPosition newPos = pos;
    
    // 1. calculate the new tile relative x and y
    newPos.tileX = newPos.tileX + generalizeCoords( &newPos.x, world->tileWidth );
    newPos.tileY = newPos.tileY + generalizeCoords( &newPos.y, world->tileHeight );

    // 2. calculate the new tile x and y and update tilemap x and y
    newPos.tileMapX = newPos.tileMapX + generalizeTileIndex( &newPos.tileX, world->tileCountX );
    newPos.tileMapY = newPos.tileMapY + generalizeTileIndex( &newPos.tileY, world->tileCountY );

    return newPos;
}

bool isMoveAllowed( GeneralizedPosition newPos, World*  world )
{
    bool allowed = false;

    TileMap* map = getMap( world, newPos.tileMapX, newPos.tileMapY );
    
    if( map )
    {
        if( (newPos.tileX >= 0) && (newPos.tileX < world->tileCountX) &&
            (newPos.tileY >= 0) && (newPos.tileY < world->tileCountY) )
        {
            allowed = ( map->tiles[newPos.tileY * world->tileCountX + newPos.tileX] == 0 ); 
        }
    }
    
    return allowed;
}

void updatePlayer( UserInput* input, Player* player, World* world, f32 dt )
{
    f32 playerX  = player->playerPos.x;
    f32 playerY  = player->playerPos.y;

    f32 movement = dt * player->speed;

    if( input->arrowUp.isDown )    
    {
        playerY -= movement;
    }
    if( input->arrowDown.isDown )  
    {
        playerY += movement;
    }
    if( input->arrowRight.isDown ) 
    {
        playerX += movement;
    }
    if( input->arrowLeft.isDown )  
    {
        playerX -= movement;
    }

    GeneralizedPosition p = player->playerPos;

    GeneralizedPosition bottomLeft  = p;
    GeneralizedPosition bottomRight = p;
    GeneralizedPosition topLeft     = p;
    GeneralizedPosition topRight    = p;

    bottomLeft.x  = playerX - player->width * 0.5f;
    bottomLeft.y  = playerY + player->height * 0.5f;

    bottomRight.x = playerX + player->width * 0.5f;
    bottomRight.y = playerY + player->height * 0.5f;

    topLeft.x     = playerX - player->width * 0.5f;
    topLeft.y     = playerY - player->height * 0.5f;

    topRight.x    = playerX + player->width * 0.5f;
    topRight.y    = playerY - player->height * 0.5f;

    bottomLeft  = getGeneralizedPosition( world, bottomLeft );
    bottomRight = getGeneralizedPosition( world, bottomRight );
    topLeft     = getGeneralizedPosition( world, topLeft );
    topRight    = getGeneralizedPosition( world, topRight );

    if( isMoveAllowed( bottomLeft,  world ) &&
        isMoveAllowed( bottomRight, world ) &&
        isMoveAllowed( topLeft,     world ) &&
        isMoveAllowed( topRight,    world )  )
    {
        p.x = playerX;
        p.y = playerY;
        player->playerPos = getGeneralizedPosition( world, p );
    }
}

void UpdateAndRender( ApplicationMemory* memory, 
                      RenderBuffer*      buffer,  
                      UserInput*         input,
                      PlatformInfo*      info )
{
    ApplicationState* state  = (ApplicationState*)memory->permanentMemory;
    Player*           player = &state->player;
    World*            world  = &state->world;

    // check if the ESC key was pressed
    if( input->esc.isDown )
    {
        state->reload = true;
    }

    if( !memory->isInitialized || state->reload )
    {
        world->tilemapCountX  = TILEMAPS_X;
        world->tilemapCountY  = TILEMAPS_Y;
        world->tileCountX     = TILEMAP_X;
        world->tileCountY     = TILEMAP_Y;
        world->tileWidth      = TILE_WIDTH;
        world->tileHeight     = TILE_WIDTH;

        Color playerColor = { 0.8, 0.8, 1.0, 1.0 };

        player->playerPos.x        = world->tileWidth * 0.5f;
        player->playerPos.y        = world->tileHeight * 0.5f;
        player->playerPos.tileMapX = 0;
        player->playerPos.tileMapY = 0;
        player->playerPos.tileX    = 1;
        player->playerPos.tileY    = 1;
        player->height             = 30.0f;
        player->width              = 30.0f;
        player->speed              = 200;
        player->color              = playerColor;
        
        info->debugMode       = false; // set this to true to get platform debug info printed to stdout
        memory->isInitialized = true;
        state->reload         = false;
    }

    buildWorld( world );

    updatePlayer( input, player, &state->world, info->deltaTimeS );

    Color background = { 0.9f, 0.2f, 0.8f, 1.0f };
    drawRectangle( buffer, 0, 0, buffer->width, buffer->height, background );
    drawMap( buffer, &state->world, &state->player );
    drawPlayer( buffer, player, world );
}