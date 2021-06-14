#include "SB_Application.h"
#include "SB_Intrinsics.h"
#include "SB_Tilemap.h"
#include "SB_Tilemap.cpp"

#define PushStruct( pool, struct ) (struct*)PushStruct_( pool, sizeof(struct) )
void* PushStruct_( MemoryPool* pool, memory_index size )
{
    Assert( pool->usedBytes + size <= pool->size );
    u8* allocatedMemory = pool->base + pool->usedBytes;

    pool->usedBytes += size;

    return allocatedMemory;
}

void buildWorld( TileMap* map )
{
    
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

void drawPlayer( RenderBuffer* buffer, Player* player, TileMap* tilemap )
{
    DecomposedPosition pos = decomposePosition( player->playerPos );

    f32 tileX = pos.tileX * tilemap->tileInMeters;
    f32 tileY = pos.tileY * tilemap->tileInMeters;

    f32 minx  = tilemap->metersToPixels * ( tileX + player->playerPos.x - player->width/2 );
    f32 maxx  = tilemap->metersToPixels * ( tileX + player->playerPos.x + player->width/2 );
    f32 miny  = buffer->height - tilemap->metersToPixels * ( tileY + player->playerPos.y - player->height/2 );
    f32 maxy  = miny - (tilemap->metersToPixels * player->height);
    drawRectangle( buffer, minx, maxy, maxx, miny, player->color );
}

void drawMap( RenderBuffer* buffer, TileMap* tilemap, Player* player )
{
    Color tilecolor = { 0.4f, 0.4f, 0.4f, 1.0f };
    u32 tileWidth   = tilemap->tileInMeters * tilemap->metersToPixels;
    u32 tileHeight  = tilemap->tileInMeters * tilemap->metersToPixels;
    u32 tilesPerRow = tilemap->tileCountX;
    u32 rows        = tilemap->tileCountY;

    DecomposedPosition pos = decomposePosition( player->playerPos );

    TileArea* area = getTileArea( tilemap, pos.tileareaX, pos.tileareaY );
    Assert( area );

    for( u32 row=0; row<rows; ++row )
    {
        for( u32 col=0; col<tilesPerRow; ++col )
        {
            u32 tile = area->tiles[row * tilemap->tileCountX + col];
            if( tile == 1 )
            {
                u32 minX = col  * tileWidth;
                u32 maxX = minX + tileWidth;

                u32 minY = buffer->height - (row  * tileHeight);
                u32 maxY = minY - tileHeight;

                drawRectangle( buffer, minX, maxY, maxX, minY, tilecolor );
            }
        }
    }
}

void updatePlayer( UserInput* input, Player* player, TileMap* tilemap, f32 dt )
{
    f32 playerX  = player->playerPos.x;
    f32 playerY  = player->playerPos.y;

    f32 movement = dt * player->speed; // s * m/s = m -> so movement is a displacement in meters

    if( input->arrowUp.isDown )    
    {
        playerY += movement;
    }
    if( input->arrowDown.isDown )  
    {
        playerY -= movement;
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

    bottomLeft  = getGeneralizedPosition( tilemap, bottomLeft );
    bottomRight = getGeneralizedPosition( tilemap, bottomRight );
    topLeft     = getGeneralizedPosition( tilemap, topLeft );
    topRight    = getGeneralizedPosition( tilemap, topRight );

    if( isMoveAllowed( bottomLeft,  tilemap ) &&
        isMoveAllowed( bottomRight, tilemap ) &&
        isMoveAllowed( topLeft,     tilemap ) &&
        isMoveAllowed( topRight,    tilemap )  )
    {
        p.x = playerX;
        p.y = playerY;
        player->playerPos = getGeneralizedPosition( tilemap, p );
    }
}

void UpdateAndRender( ApplicationMemory* memory, 
                      RenderBuffer*      buffer,  
                      UserInput*         input,
                      PlatformInfo*      info )
{
    /************************************
     * MEMORY LAYOUT -> PERMANENT STORE
     * --------------------------------
     * 
     * -> ApplicationState
     * -> MemoryPool [TileMap]
     ************************************/

    ApplicationState* state   = (ApplicationState*)memory->permanentMemory;
    Player*           player  = &state->player;
    Screen*           screen  = &state->screen;

    // check if the ESC key was pressed
    if( input->esc.isDown )
    {
        info->reload = true;
    }

    MemoryPool* tilemapMemory = (MemoryPool*)memory->permanentMemory + sizeof(ApplicationState);
    TileMap*    tilemap       = nullptr;

    if( !memory->isInitialized || info->reload )
    {
        tilemapMemory->size         = MegaBytes(64);
        tilemapMemory->base         = (u8*)tilemapMemory + sizeof( MemoryPool );
        tilemapMemory->usedBytes    = 0;

        tilemap = PushStruct( tilemapMemory, TileMap );
        tilemap->tileCountX         = TILEMAP_X;
        tilemap->tileCountY         = TILEMAP_Y;
        tilemap->tileInMeters       = 2.0f;
        tilemap->tileInPixels       = TILE_WIDTH;
        tilemap->metersToPixels     = (f32)((f32)tilemap->tileInPixels/tilemap->tileInMeters);

        Color playerColor = { 0.8, 0.8, 1.0, 1.0 };

        player->playerPos.unifiedPositionX = 1;
        player->playerPos.unifiedPositionY = 1;
        player->playerPos.x        = tilemap->tileInMeters * 0.5f;
        player->playerPos.y        = tilemap->tileInMeters * 0.5f + 1.0f;
        player->width              = 0.9f * tilemap->tileInMeters;
        player->height             = 0.9f * tilemap->tileInMeters;
        player->speed              = 4.0f;
        player->color              = playerColor;

        screen->tilesInX           = SCREEN_X;
        screen->tilesInY           = SCREEN_Y;
        
        info->debugMode       = false; // set this to true to get platform debug info printed to stdout
        memory->isInitialized = true;
    }

    updatePlayer( input, player, tilemap, info->deltaTimeS );

    Color background = { 0.9f, 0.2f, 0.8f, 1.0f };
    drawRectangle( buffer, 0, 0, buffer->width, buffer->height, background );
    drawMap( buffer, tilemap, player );
    drawPlayer( buffer, player, tilemap );
}