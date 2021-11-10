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

void drawImage( RenderBuffer* buffer, v2 position, Image* img )
{
    u32 screenWidth  = buffer->width;
    u32 screenHeight = buffer->height;

    s32 xmin = roundToS32( position.x );
    s32 ymin = roundToS32( position.y );
    s32 xmax = roundToS32( position.x + img->width );
    s32 ymax = roundToS32( position.y + img->height );

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

void drawBackground( RenderBuffer* buffer )
{
    Color backgroundColor = { 25.0f/255.0f, 25.0f/255.0f, 25.0f/255.0f, 1.0f };
    drawRectangle( buffer, V2(0, 0), V2(buffer->width, buffer->height), backgroundColor );
}

void drawPlayer( RenderBuffer* buffer, Player* player, TileMap* tilemap )
{
    DecomposedPosition pos = decomposePosition( player->playerPos );
    
    u32 tileOffsetX = pos.tileareaX * tilemap->tileCountX;
    u32 tileOffsetY = pos.tileareaY * tilemap->tileCountY;

    f32 tileY = pos.tileY * tilemap->tileInMeters;
    f32 tileX = pos.tileX * tilemap->tileInMeters;

    Color tilecolor = { 75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 1.0f };

    f32 tileMinx = tilemap->metersToPixels * tileX;
    f32 tileMaxx = tilemap->metersToPixels * (tileX + tilemap->tileInMeters);
    f32 tileMiny = buffer->height - tilemap->metersToPixels * tileY;
    f32 tileMaxy = tileMiny - (tilemap->metersToPixels * tilemap->tileInMeters);
    drawRectangle( buffer, V2(tileMinx, tileMaxy), V2(tileMaxx, tileMiny), tilecolor );

    f32 minx  = tilemap->metersToPixels * ( tileX + player->playerPos.tileRelative.x - player->size.x/2 );
    f32 maxx  = tilemap->metersToPixels * ( tileX + player->playerPos.tileRelative.x + player->size.x/2 );
    f32 miny  = buffer->height - tilemap->metersToPixels * ( tileY + player->playerPos.tileRelative.y - player->size.y/2 );
    f32 maxy  = miny - (tilemap->metersToPixels * player->size.y);
    drawRectangle( buffer, V2(minx, maxy), V2(maxx, miny), player->color );
}

void drawTileArea( RenderBuffer* buffer, TileMap* tilemap, TileArea* area )
{
    Color tilecolor = { 53.0f/255.0f, 51.0f/255.0f, 47.0f/255.0f, 1.0f };

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

                //drawImage( buffer, V2(minX, maxY), tilemap->brickImage );
                drawRectangle( buffer, V2(minX, maxY), V2(maxX, minY), tilecolor );
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

bool wallCollision( f32 wallCoord, 
                    f32 movementX, f32 movementY, 
                    f32 p0X, f32 p0Y, 
                    f32* collisionTime, 
                    f32 ymin, f32 ymax )
{
    bool hit = false;
    static f32 epsilon = 0.0001f;
    if( movementX != 0.0f )
    {
        f32 t = (wallCoord - p0X) / movementX;
        f32 y = p0Y + (t * movementY);

        if( (t >= 0.0f) && (*collisionTime > t) )
        {
            if( y >= ymin && y <= ymax  )
            {
                *collisionTime = maximum( 0.0f, t - epsilon );
                hit = true;                
            }                
        }
    }

    return hit;
}

void collisionDetection( Player*  player, 
                         TileMap* tilemap, 
                         v2       direction, 
                         f32      acceleration, 
                         f32      dt )
{
    static v2 north = V2(  0.0f,  1.0f );
    static v2 south = V2(  0.0f, -1.0f );
    static v2 east  = V2(  1.0f,  0.0f );
    static v2 west  = V2( -1.0f,  0.0f );

    f32 tileWidth   = tilemap->tileInMeters;
    f32 tileHeight  = tileWidth;

    DecomposedPosition oldPosition = decomposePosition( player->playerPos );
    TileArea*          tileArea    = getTileArea( tilemap, oldPosition.tileareaX, oldPosition.tileareaY );
    
    v2 accelerationVector          = direction * acceleration - (4.0f * player->velocityVector);
    v2 velocity                    = ((2.0f * accelerationVector) * dt) + player->velocityVector;
    v2 playerSize                  = player->size;
    player->velocityVector         = velocity;
    
    // equations of motion:
    // p' = 1/2at^2 + vt + p -> acceleration based position
    // v' = 2at + v          -> acceleration based velocity ( 1st derivative of the position  )
    // a' = a                -> acceleration                ( 2nd derivative of the velocity )
    v2 movement = ((0.5f * accelerationVector) * square( dt )) + (velocity * dt);
    
    // 4. calculate time of collision and the collision point
    f32 timeRemaining = 1.0f;
    v2  wallNormal      = V2(0,0);

    // create the new position
    GeneralizedPosition newPos = buildNewPosition( oldPosition, movement, tilemap );
    DecomposedPosition  newPosition = decomposePosition( newPos );

    // 1. constuct the search area
    s32 searchTilesMinX = (oldPosition.tileX < newPosition.tileX) ? oldPosition.tileX : newPosition.tileX;
    s32 searchTilesMinY = (oldPosition.tileY < newPosition.tileY) ? oldPosition.tileY : newPosition.tileY;
    s32 searchTilesMaxX = (oldPosition.tileX < newPosition.tileX) ? newPosition.tileX : oldPosition.tileX;
    s32 searchTilesMaxY = (oldPosition.tileY < newPosition.tileY) ? newPosition.tileY : oldPosition.tileY;

    // adjust the search area for the minkowski sum
    s32 playerTileWidth  = ceilToS32( player->size.x / tileWidth );
    s32 playerTileHeigth = ceilToS32( player->size.y / tileHeight );

    searchTilesMinX -= playerTileWidth;
    searchTilesMaxX += playerTileWidth;
    searchTilesMinY -= playerTileHeigth;
    searchTilesMaxY += playerTileHeigth;

    for( u32 iteration=0; (iteration < 4) && (timeRemaining > 0.0f); ++iteration )
    {
        f32 t = 1.0f;
        TileArea* area = nullptr;

        for( s32 searchY = searchTilesMinY; searchY <= searchTilesMaxY; ++searchY )
        {
            for( s32 searchX = searchTilesMinX; searchX <= searchTilesMaxX; ++searchX )
            {
                s32 testX = buildNewTileCoord( searchX, (s32)tilemap->tileCountX );
                s32 testY = buildNewTileCoord( searchY, (s32)tilemap->tileCountY );

                u32 areaX = (u32)(testX / tilemap->tileCountX);
                u32 areaY = (u32)(testY / tilemap->tileCountY);

                area = getTileArea( tilemap, areaX, areaY );

                // create the minkowski sum out of the wall tile and the player rectangle
                v2 minCorner = V2( testX * tileWidth, testY * tileHeight ) - (playerSize * 0.5f);
                v2 maxCorner = V2( (testX * tileWidth) + tileWidth, (testY * tileHeight) + tileHeight ) + (playerSize * 0.5f);
                v2 p0        = V2( oldPosition.tileX * tileWidth, oldPosition.tileY * tileHeight ) + oldPosition.tileRelative;

                PrintTile(testX, testY, areaX, areaY);

                if( getTileValue( tilemap, area, testX, testY ) == 1 )
                {
                    if( wallCollision( maxCorner.y, movement.y, movement.x, p0.y, p0.x, &t, minCorner.x, maxCorner.x ) )
                    {
                        Print( "collision north" );
                        wallNormal = north;
                    }
                    if( wallCollision( minCorner.y, movement.y, movement.x, p0.y, p0.x, &t, minCorner.x, maxCorner.x ) )
                    {
                        wallNormal = south;
                        Print( "collision south" );
                    }
                    if( wallCollision( minCorner.x, movement.x, movement.y, p0.x, p0.y, &t, minCorner.y, maxCorner.y ) )
                    {
                        wallNormal = west;
                        Print( "collision west" );
                    }
                    if( wallCollision( maxCorner.x, movement.x, movement.y, p0.x, p0.y, &t, minCorner.y, maxCorner.y ) )
                    {
                        wallNormal = east;
                        Print( "collision east" );
                    }
                }
            }
        }

        // update the player position
        player->playerPos = buildNewPosition( oldPosition, movement * t, tilemap );
        
        // update the movement
        movement = movement - 1*movement.dot(wallNormal)*wallNormal;
        
        // update the velocity
        velocity = velocity - 1*velocity.dot(wallNormal)*wallNormal;
        player->velocityVector = velocity;
        
        // update time to collision
        timeRemaining -= timeRemaining * t;        
    }

    return;
}

void updatePlayer( UserInput* input, Player* player, TileMap* tilemap, f32 dt )
{
    v2 playerRelativePosition = player->playerPos.tileRelative;
    f32 acceleration          = input->space.isDown ? 3.0f * player->acceleration : player->acceleration;
    v2 direction              = V2(0.0f, 0.0f);
    
    if( input->arrowUp.isDown )    
    {
        direction.y = 1.0f;
    }
    if( input->arrowDown.isDown )  
    {
        direction.y = -1.0f;
    }
    if( input->arrowRight.isDown ) 
    {
        direction.x = 1.0f;
    }
    if( input->arrowLeft.isDown )  
    {
        direction.x = -1.0f;
    }

    direction.normalize();

    collisionDetection( player, tilemap, direction, acceleration, dt );
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

        Color playerColor = { 1.0f, 162.0f/255.0f, 0.0f, 1.0f };

        DecomposedPosition startposition = {0};
        startposition.tileareaX = 0;
        startposition.tileareaY = 0;
        startposition.tileX     = 2;
        startposition.tileY     = 2;
        startposition.tileRelative = V2( tilemap->tileInMeters * 0.5f, tilemap->tileInMeters * 0.5f );
        
        player->playerPos       = composePosition( startposition );
        player->size.x          = 0.9f * tilemap->tileInMeters;
        player->size.y          = 0.9f * tilemap->tileInMeters;
        player->acceleration    = 50.0f; // in meters per second squared -> m/s^2
        player->velocityVector  = V2(0.0f, 0.0f);
        player->color           = playerColor;
        
        info->debugMode       = false; // set this to true to get platform debug info printed to stdout
        memory->isInitialized = true;
        state->loading        = false;
    }

    updatePlayer( input, player, tilemap, info->deltaTimeS );

    drawBackground( buffer );
    drawWorld( buffer, tilemap, player );
    drawPlayer( buffer, player, tilemap );
}
