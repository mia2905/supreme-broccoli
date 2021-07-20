#include "SB_Application.h"
#include "SB_Intrinsics.h"
#include "SB_Tilemap.h"
#include "SB_Tilemap.cpp"

void buildWorld( MemoryPool* pool, TileMap* map )
{
    u32 x = 0;
    u32 y = 0;

    for( u32 i=0; i<NR_OF_TILEAREAS; ++i )
    {
        buildTileArea( pool, map, x, y );
        if( randomNumber() % 2 == 0 )
            x++;
        else
            y++;
    }
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

void drawPlayer( RenderBuffer* buffer, Player* player, TileMap* tilemap, Screen* screen )
{
    DecomposedPosition pos = decomposePosition( player->playerPos );
    DecomposedPosition origin = decomposePosition( screen->origin );

    u32 tileOffsetX = (pos.tileareaX - origin.tileareaX) * tilemap->tileCountX;
    u32 tileOffsetY = (pos.tileareaY - origin.tileareaY) * tilemap->tileCountY;

    f32 tileY = (tileOffsetY + pos.tileY) * tilemap->tileInMeters;
    f32 tileX = (tileOffsetX + pos.tileX) * tilemap->tileInMeters;

    Color tilecolor = { 1.0f, 0.0f, 0.0f, 1.0f };

    f32 tileMinx = tilemap->metersToPixels * tileX;
    f32 tileMaxx = tilemap->metersToPixels * (tileX + tilemap->tileInMeters);
    f32 tileMiny = buffer->height - tilemap->metersToPixels * tileY;
    f32 tileMaxy = tileMiny - (tilemap->metersToPixels * tilemap->tileInMeters);
    drawRectangle( buffer, tileMinx, tileMaxy, tileMaxx, tileMiny, tilecolor );


    f32 minx  = tilemap->metersToPixels * ( tileX + player->playerPos.x - player->width/2 );
    f32 maxx  = tilemap->metersToPixels * ( tileX + player->playerPos.x + player->width/2 );
    f32 miny  = buffer->height - tilemap->metersToPixels * ( tileY + player->playerPos.y - player->height/2 );
    f32 maxy  = miny - (tilemap->metersToPixels * player->height);
    drawRectangle( buffer, minx, maxy, maxx, miny, player->color );
}

void drawTileArea( RenderBuffer* buffer, TileMap* tilemap, TileArea* area, u32 screenTileX, u32 screenTileY )
{
    Color tilecolor = { 0.4f, 0.4f, 0.4f, 1.0f };

    u32 tileWidth   = tilemap->tileInMeters * tilemap->metersToPixels;
    u32 tileHeight  = tilemap->tileInMeters * tilemap->metersToPixels;
    u32 tilesPerRow = tilemap->tileCountX;
    u32 rows        = tilemap->tileCountY;

    for( u32 row=0; row<rows; ++row )
    {
        for( u32 col=0; col<tilesPerRow; ++col )
        {
            u32 tile = area->tiles[row * tilemap->tileCountX + col];
            if( tile == 1 )
            {
                u32 minX = (col + screenTileX) * tileWidth;
                u32 maxX = minX + tileWidth;

                u32 minY = buffer->height - ( (row + screenTileY) * tileHeight);
                u32 maxY = minY - tileHeight;

                drawRectangle( buffer, minX, maxY, maxX, minY, tilecolor );
            }
        }
    }
}

void drawWorld( RenderBuffer* buffer, TileMap* world, Screen* screen )
{
    DecomposedPosition currentScreen = decomposePosition( screen->origin );
    u32 areaX = currentScreen.tileareaX;
    u32 areaY = currentScreen.tileareaY;
    u32 screenX = 0;
    u32 screenY = 0;

    while( screenY < screen->tilesInY )
    {
        screenX = 0;
        while( screenX < screen->tilesInX )
        {
            TileArea* area = getTileArea( world, areaX, areaY );
            if( area->tiles != nullptr )
            {
                drawTileArea( buffer, world, area, screenX, screenY );
            }
            
            screenX += world->tileCountX;
            areaX++;
        }

        screenY += world->tileCountY;
        areaY++;
    }
}

void updatePlayer( UserInput* input, Player* player, TileMap* tilemap, f32 dt )
{
    f32 playerX  = player->playerPos.x;
    f32 playerY  = player->playerPos.y;
    f32 speed    = input->space.isDown ? 3.0f * player->speed : player->speed;

    f32 movement = dt * speed; // s * m/s = m -> so movement is a displacement in meters

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

    // old player position
    GeneralizedPosition p = player->playerPos;

    // new positions of the four corners
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
        // update old position with new x and y
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

    ApplicationState* state      = (ApplicationState*)memory->permanentMemory;
    Player*           player     = &state->player;
    Screen*           screen     = &state->screen;
    MemoryPool*       tileMemory = &state->tileMemory;
    TileMap*          tilemap    = &state->tilemap;


    // check if the ESC key was pressed
    if( input->esc.isDown )
    {
        info->reload = true;
    }

    if( !memory->isInitialized || info->reload )
    {
        initMath();

        tileMemory->size            = MegaBytes(64);
        tileMemory->base            = (u8*)memory->permanentMemory + sizeof( ApplicationState );
        tileMemory->usedBytes       = 0;

        tilemap->tileCountX         = TILEMAP_X;
        tilemap->tileCountY         = TILEMAP_Y;
        tilemap->tileInMeters       = 2.0f;
        tilemap->tileInPixels       = TILE_WIDTH;
        tilemap->metersToPixels     = (f32)((f32)tilemap->tileInPixels/tilemap->tileInMeters);
        tilemap->tileAreas          = PushArray( tileMemory, 
                                                 NR_OF_TILEAREAS * NR_OF_TILEAREAS,
                                                 TileArea );

        buildWorld( tileMemory, tilemap );                                                 

        Color playerColor = { 0.8, 0.8, 1.0, 1.0 };

        player->playerPos.unifiedPositionX = 1;
        player->playerPos.unifiedPositionY = 1;
        player->playerPos.x                = tilemap->tileInMeters * 0.5f;
        player->playerPos.y                = tilemap->tileInMeters * 0.5f + 1.0f;
        player->width                      = 0.9f * tilemap->tileInMeters;
        player->height                     = 0.9f * tilemap->tileInMeters;
        player->speed                      = 4.0f;
        player->color                      = playerColor;
        
        screen->tilesInX                   = SCREEN_X;
        screen->tilesInY                   = SCREEN_Y;
        screen->origin.unifiedPositionX    = 0;
        screen->origin.unifiedPositionY    = 0;
        
        info->debugMode       = false; // set this to true to get platform debug info printed to stdout
        memory->isInitialized = true;
    }

    updatePlayer( input, player, tilemap, info->deltaTimeS );

    Color background = { 0.9f, 0.2f, 0.8f, 1.0f };
    drawRectangle( buffer, 0, 0, buffer->width, buffer->height, background );
    drawWorld( buffer, tilemap, screen );
    drawPlayer( buffer, player, tilemap, screen );
}
