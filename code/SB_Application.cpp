#include "SB_Application.h"
#include "SB_Intrinsics.h"
#include "SB_Tilemap.h"
#include "SB_Tilemap.cpp"

void buildWorld( MemoryPool* pool, TileMap* map )
{
    u32 x = 0;
    u32 y = 0;

    DOOR_DIRECTION nextRoomEntry = NONE;
    DOOR_DIRECTION door          = NONE;

    for( u32 i=0; i<NR_OF_TILEAREAS; ++i )
    {
        TileArea* area = buildTileArea( pool, map, x, y );
        door      = (DOOR_DIRECTION)(randomNumber() % 2 + 2);

        if( nextRoomEntry != NONE )
        {
            setDoor( area, nextRoomEntry );
        }

        switch( door ) 
        {
            case RIGHT:  
                ++x; 
                nextRoomEntry = LEFT;   
                break;
            case TOP:    
                ++y; 
                nextRoomEntry = BOTTOM; 
                break;
            
            default:
                Assert( door == LEFT || door == BOTTOM );
                break;
        }    
         
        setDoor( area, door );
    }
}

void drawImage( RenderBuffer* buffer, f32 x, f32 y, Image* img )
{
    u32 screenWidth  = buffer->width;
    u32 screenHeight = buffer->height;

    s32 xmin = roundToS32( x );
    s32 ymin = roundToS32( y );
    s32 xmax = roundToS32( x + img->width );
    s32 ymax = roundToS32( y + img->height );

    if( xmin < 0 ) xmin = 0;
    if( xmax < 0 ) xmax = 0;
    if( xmin >= screenWidth ) return; // offscreen
    if( xmax >= screenWidth ) xmax = screenWidth;

    if( ymin < 0 ) ymin = 0;
    if( ymax < 0 ) ymax = 0;
    if( ymin >= screenHeight ) return; // offscreen
    if( ymax >= screenHeight ) ymax = screenHeight;

    u8* src = img->data;

    // the start of the rectangle is the pixel at minX / minY
    for( u32 row=ymin; row<ymax; ++row )
    {
        u8* base = buffer->buffer + (u32)((row * buffer->width + xmin) * sizeof(Pixel));
        Pixel* p = (Pixel*)base;
        for( u32 col=xmin; col<xmax; ++col )
        {
            p->red   = (u8)src[0];
            p->green = (u8)src[1];
            p->blue  = (u8)src[2];
            p->alpha = (u8)src[3];
            ++p;
            src += 4;
        }
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

void drawBackground( RenderBuffer* buffer )
{
    Color backgroundColor = { 0.3f, 0.3f, 0.3f, 1.0f };
    drawRectangle( buffer, 0, 0, buffer->width, buffer->height, backgroundColor );
}

void drawPlayer( RenderBuffer* buffer, Player* player, TileMap* tilemap )
{
    DecomposedPosition pos = decomposePosition( player->playerPos );
    
    u32 tileOffsetX = pos.tileareaX * tilemap->tileCountX;
    u32 tileOffsetY = pos.tileareaY * tilemap->tileCountY;

    f32 tileY = pos.tileY * tilemap->tileInMeters;
    f32 tileX = pos.tileX * tilemap->tileInMeters;

    Color tilecolor = { 1.0f, 0.0f, 0.0f, 0.5f };

    f32 tileMinx = tilemap->metersToPixels * tileX;
    f32 tileMaxx = tilemap->metersToPixels * (tileX + tilemap->tileInMeters);
    f32 tileMiny = buffer->height - tilemap->metersToPixels * tileY;
    f32 tileMaxy = tileMiny - (tilemap->metersToPixels * tilemap->tileInMeters);
    drawRectangle( buffer, tileMinx, tileMaxy, tileMaxx, tileMiny, tilecolor );

    f32 minx  = tilemap->metersToPixels * ( tileX + player->playerPos.tileRelative.x - player->size.x/2 );
    f32 maxx  = tilemap->metersToPixels * ( tileX + player->playerPos.tileRelative.x + player->size.x/2 );
    f32 miny  = buffer->height - tilemap->metersToPixels * ( tileY + player->playerPos.tileRelative.y - player->size.y/2 );
    f32 maxy  = miny - (tilemap->metersToPixels * player->size.y);
    drawRectangle( buffer, minx, maxy, maxx, miny, player->color );
}

void drawTileArea( RenderBuffer* buffer, TileMap* tilemap, TileArea* area )
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
                u32 minX = col * tileWidth;
                u32 maxX = minX + tileWidth;

                u32 minY = buffer->height - (row * tileHeight);
                u32 maxY = minY - tileHeight;

                drawImage( buffer, minX, maxY, tilemap->brickImage );
            }
        }
    }
}

void drawWorld( RenderBuffer* buffer, TileMap* world, Player* player )
{
    DecomposedPosition currentScreen = decomposePosition( player->playerPos );
    u32 areaX = currentScreen.tileareaX;
    u32 areaY = currentScreen.tileareaY;
    
    TileArea* area = getTileArea( world, areaX, areaY );
    if( area->tiles != nullptr )
    {
        drawTileArea( buffer, world, area );
    }
}

void updatePlayer( UserInput* input, Player* player, TileMap* tilemap, f32 dt )
{
    v2 playerRelativePosition = player->playerPos.tileRelative;
    f32 speed = input->space.isDown ? 3.0f * player->speed : player->speed;

    f32 movement = dt * speed; // s * m/s = m -> so movement is a displacement in meters

    if( input->arrowUp.isDown )    
    {
        playerRelativePosition.y += movement;
    }
    if( input->arrowDown.isDown )  
    {
        playerRelativePosition.y -= movement;
    }
    if( input->arrowRight.isDown ) 
    {
        playerRelativePosition.x += movement;
    }
    if( input->arrowLeft.isDown )  
    {
        playerRelativePosition.x -= movement;
    }

    // old player position
    GeneralizedPosition p = player->playerPos;

    // new positions of the four corners
    GeneralizedPosition bottomLeft  = p;
    GeneralizedPosition bottomRight = p;
    GeneralizedPosition topLeft     = p;
    GeneralizedPosition topRight    = p;

    bottomLeft.tileRelative    = playerRelativePosition - (player->size * 0.5f);
    bottomRight.tileRelative   = playerRelativePosition + V2( player->size.x * 0.5f, -player->size.y * 0.5f );
    topLeft.tileRelative       = playerRelativePosition - V2( -player->size.x * 0.5f, player->size.y * 0.5f );
    topRight.tileRelative      = playerRelativePosition + (player->size * 0.5f);

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
        p.tileRelative    = playerRelativePosition;
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

    ApplicationState* state       = (ApplicationState*)memory->permanentMemory;
    Player*           player      = &state->player;
    MemoryPool*       tileMemory  = &state->tileMemory;
    TileMap*          tilemap     = &state->tilemap;
    MemoryPool*       imageMemory = &state->imageMemory;


    // check if the ESC key was pressed
    if( input->esc.isDown )
    {
        info->reload = true;
    }

    // reentrant guard
    if( state->loading )
        return;

    // NOTE: this part of the code is NOT reentrant!!!!
    if( !memory->isInitialized || info->reload )
    {
        state->loading              = true;
        initMath();

        tileMemory->size            = MegaBytes(64);
        tileMemory->base            = (u8*)memory->permanentMemory + sizeof( ApplicationState );
        tileMemory->usedBytes       = 0;

        tilemap->tileCountX         = TILES_PER_AREA_X;
        tilemap->tileCountY         = TILES_PER_AREA_Y;
        tilemap->tileInMeters       = 2.0f;
        tilemap->tileInPixels       = TILE_SIZE;
        tilemap->metersToPixels     = (f32)((f32)tilemap->tileInPixels/tilemap->tileInMeters);
        tilemap->tileAreas          = PushArray( tileMemory, 
                                                 NR_OF_TILEAREAS * NR_OF_TILEAREAS,
                                                 TileArea );

        buildWorld( tileMemory, tilemap );             

        imageMemory->size      = MegaBytes(64);
        imageMemory->base      = (u8*)tileMemory->base + tileMemory->size;
        imageMemory->usedBytes = 0;

        const char* brickfile = "./art/bricktile.png";
        tilemap->brickImage   = state->services.loadImage( imageMemory, brickfile );

        const char* bgrFile   = "./art/background.png";
        state->background     = state->services.loadImage( imageMemory, bgrFile );

        Color playerColor = { 0.8, 0.8, 1.0, 1.0 };

        DecomposedPosition startposition = {0};
        startposition.tileareaX = 0;
        startposition.tileareaY = 0;
        startposition.tileX     = 2;
        startposition.tileY     = 2;

        composePosition( startposition, &player->playerPos );

        player->playerPos.tileRelative.x   = tilemap->tileInMeters * 0.5f;
        player->playerPos.tileRelative.y   = tilemap->tileInMeters * 0.5f + 1.0f;
        player->size.x                     = 0.9f * tilemap->tileInMeters;
        player->size.y                     = 0.9f * tilemap->tileInMeters;
        player->speed                      = 4.0f;
        player->color                      = playerColor;
        
        info->debugMode       = false; // set this to true to get platform debug info printed to stdout
        memory->isInitialized = true;
        state->loading        = false;
    }

    updatePlayer( input, player, tilemap, info->deltaTimeS );

    drawBackground( buffer );
    drawWorld( buffer, tilemap, player );
    drawPlayer( buffer, player, tilemap );
}
