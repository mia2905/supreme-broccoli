#include "SB_Application.h"
#include "SB_Intrinsics.h"
#include "SB_Math.h"
#include "SB_Render.cpp"
#include "SB_Memory.cpp"
#include "SB_Tilemap.cpp"
#include "SB_Sound.cpp"

struct Player
{
    GeneralizedPosition playerPos;
    v2     size;
    v2     velocityVector;
    f32    acceleration; // meters per second squared -> m/s^2
    Color  color;
    Image* playerImg;
};

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

void drawBackground( RenderBuffer* buffer )
{
    Color backgroundColor = { 200.0f/255.0f, 200.0f/255.0f, 200.0f/255.0f, 1.0f };
    drawRectangle( buffer, vec2(0, 0), vec2(buffer->width, buffer->height), backgroundColor );
}

void drawPlayer( RenderBuffer* buffer, Player* player, TileMap* tilemap )
{
    DecomposedPosition pos = decomposePosition( player->playerPos );
    
    u32 tileOffsetX = pos.tileareaX * tilemap->tileCountX;
    u32 tileOffsetY = pos.tileareaY * tilemap->tileCountY;

    f32 tileY = pos.tileY * tilemap->tileInMeters;
    f32 tileX = pos.tileX * tilemap->tileInMeters;

    Color tilecolor = { 75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.9f };

    f32 tileMinx = tilemap->metersToPixels * tileX;
    f32 tileMaxx = tilemap->metersToPixels * (tileX + tilemap->tileInMeters);
    f32 tileMiny = buffer->height - tilemap->metersToPixels * tileY;
    f32 tileMaxy = tileMiny - (tilemap->metersToPixels * tilemap->tileInMeters);
    drawRectangle( buffer, vec2(tileMinx, tileMaxy), vec2(tileMaxx, tileMiny), tilecolor );

    f32 minx  = tilemap->metersToPixels * ( tileX + player->playerPos.tileRelative.x - player->size.x/2 );
    f32 maxx  = tilemap->metersToPixels * ( tileX + player->playerPos.tileRelative.x + player->size.x/2 );
    f32 miny  = buffer->height - tilemap->metersToPixels * ( tileY + player->playerPos.tileRelative.y - player->size.y/2 );
    f32 maxy  = miny - (tilemap->metersToPixels * player->size.y);
    drawRectangle( buffer, vec2(minx, maxy), vec2(maxx, miny), player->color );

    drawImage( buffer, vec2(minx, maxy), player->playerImg );
}

void drawTileArea( RenderBuffer* buffer, TileMap* tilemap, TileArea* area )
{
    Color tilecolor = { 53.0f/255.0f, 51.0f/255.0f, 47.0f/255.0f, 0.8f };

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
                drawRectangle( buffer, vec2(minX, maxY), vec2(maxX, minY), tilecolor );
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
    static v2 north = vec2(  0.0f,  1.0f );
    static v2 south = vec2(  0.0f, -1.0f );
    static v2 east  = vec2(  1.0f,  0.0f );
    static v2 west  = vec2( -1.0f,  0.0f );

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
    v2  wallNormal      = vec2(0,0);

    // create the new position
    GeneralizedPosition newPos = buildNewPosition( oldPosition, movement, tilemap );
    DecomposedPosition  newPosition = decomposePosition( newPos );

    u32 oldAbsTileX = oldPosition.tileX + oldPosition.tileareaX * tilemap->tileCountX;
    u32 oldAbsTileY = oldPosition.tileY + oldPosition.tileareaY * tilemap->tileCountY;

    u32 newAbsTileX = newPosition.tileX + newPosition.tileareaX * tilemap->tileCountX;
    u32 newAbsTileY = newPosition.tileY + newPosition.tileareaY * tilemap->tileCountY;

    // 1. constuct the search area
    s32 searchTileMinX = (oldAbsTileX < newAbsTileX) ? oldAbsTileX : newAbsTileX;
    s32 searchTileMaxX = (oldAbsTileX > newAbsTileX) ? oldAbsTileX : newAbsTileX;
    s32 searchTileMinY = (oldAbsTileY < newAbsTileY) ? oldAbsTileY : newAbsTileY;
    s32 searchTileMaxY = (oldAbsTileY > newAbsTileY) ? oldAbsTileY : newAbsTileY;
 
    // adjust the search area for the minkowski sum
    s32 playerTileWidth  = ceilToS32( player->size.x / tileWidth );
    s32 playerTileHeigth = ceilToS32( player->size.y / tileHeight );

    searchTileMinX -= playerTileWidth;
    searchTileMaxX += playerTileWidth;
    searchTileMinY -= playerTileHeigth;
    searchTileMaxY += playerTileHeigth;
    
    for( u32 iteration=0; (iteration < 4) && (timeRemaining > 0.0f); ++iteration )
    {
        f32 t = 1.0f;
        TileArea* area = nullptr;

        for( s32 searchY = searchTileMinY; searchY <= searchTileMaxY; ++searchY )
        {
            for( s32 searchX = searchTileMinX; searchX <= searchTileMaxX; ++searchX )
            {
                // create the minkowski sum out of the wall tile and the player rectangle
                v2 minCorner = vec2( searchX * tileWidth, searchY * tileHeight ) - (playerSize * 0.5f);
                v2 maxCorner = vec2( (searchX * tileWidth) + tileWidth, (searchY * tileHeight) + tileHeight ) + (playerSize * 0.5f);
                v2 p0        = vec2( oldAbsTileX * tileWidth, oldAbsTileY * tileHeight ) + oldPosition.tileRelative;

                u32 areaX = (u32)(searchX / tilemap->tileCountX);
                u32 areaY = (u32)(searchY / tilemap->tileCountY);

                area = getTileArea( tilemap, areaX, areaY );
                s32 testX = buildNewTileCoord( searchX, tilemap->tileCountX );
                s32 testY = buildNewTileCoord( searchY, tilemap->tileCountY );

                if( getTileValue( tilemap, area, testX, testY ) == 1 )
                {
                    //Print( "\n" );
                    //Print( "search: " ); PrintTile( searchX, searchY, areaX, areaY );
                    //Print( "test: " ); PrintTile( testX, testY, areaX, areaY );

                    if( wallCollision( maxCorner.y, movement.y, movement.x, p0.y, p0.x, &t, minCorner.x, maxCorner.x ) )
                    {
                        //Print( "COLLISION NORTH\n" );
                        wallNormal = north;
                    }
                    if( wallCollision( minCorner.y, movement.y, movement.x, p0.y, p0.x, &t, minCorner.x, maxCorner.x ) )
                    {
                        //Print( "COLLISION SOUTH\n" );
                        wallNormal = south;
                    }
                    if( wallCollision( minCorner.x, movement.x, movement.y, p0.x, p0.y, &t, minCorner.y, maxCorner.y ) )
                    {
                        //Print( "COLLISION WEST\n" );
                        wallNormal = west;
                    }
                    if( wallCollision( maxCorner.x, movement.x, movement.y, p0.x, p0.y, &t, minCorner.y, maxCorner.y ) )
                    {
                        //Print( "COLLISION EAST\n" );
                        wallNormal = east;
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
    v2 direction              = vec2(0.0f, 0.0f);
    
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

void processInput( UserInput* input, ApplicationState* state )
{
    if( input->command.isDown )
    {
        state->fullscreen = !state->fullscreen;
        state->services.toggleFullScreen(state->fullscreen);
    }
}

void UpdateAndRender( ApplicationMemory* memory, 
                      RenderBuffer*      buffer,  
                      UserInput*         input )
{
    /**
        ++ ApplicationState ++

        -  MemoryPool*       appMemory; // equal to the start of ApplicationState
        -  PlatformServices  services;
        -  TileMap*          tilemap;
        -  Player*           player;
        -  File*             backgroundMp3;
        -  Mp3Buffer*        mp3Samples;
        -  bool              loading;
     */
    ApplicationState* state = (ApplicationState*)memory->permanentMemory;
    
    MemoryPool*       appMemory   = state->appMemory;
    TileMap*          tilemap     = state->tilemap;
    Player*           player      = state->player;

    processInput( input, state );

    // reentrant guard
    if( state->loading )
        return;

    // NOTE: this part of the code is NOT reentrant!!!!
    if( !memory->isInitialized )
    {
        state->loading = true;
        initMath();

        // setup the permanent memory
        /*********************
         *  
         *  MEMORY LAYOUT
         *  
         *  ApplicationState
         *  MemoryPool
         *  Memory of MemoryPool
         * 
         *********************/ 

        state->appMemory                   = (MemoryPool*)( state + sizeof( ApplicationState ) );;
        state->appMemory->base             = (u8*)( state->appMemory + sizeof( MemoryPool ) );
        state->appMemory->size             = MegaBytes( 200 );
        state->appMemory->usedBytes        = 0;

        appMemory = state->appMemory;

        // create the tilemap in permanent memory
        state->tilemap                     = PushStruct( appMemory, TileMap );
        state->player                      = PushStruct( appMemory, Player );
        tilemap = state->tilemap;
        player  = state->player;

        state->tilemap->tileCountX         = TILES_PER_AREA_X;
        state->tilemap->tileCountY         = TILES_PER_AREA_Y;
        state->tilemap->tileInMeters       = 2.0f;
        state->tilemap->tileInPixels       = TILE_SIZE;
        state->tilemap->metersToPixels     = (f32)((f32)state->tilemap->tileInPixels/state->tilemap->tileInMeters);
        state->tilemap->tileAreas          = PushArray( appMemory, 
                                                 NR_OF_TILEAREAS * NR_OF_TILEAREAS,
                                                 TileArea );

        buildWorld( appMemory, tilemap );             

        const char* playerfile   = "./art/player.png";
        const char* mp3File      = "./assets/music/datahop.mp3";

        player->playerImg        = state->services.loadImage( appMemory, playerfile );
        state->mp3Samples        = state->services.loadMp3( appMemory, mp3File );
        
        Color playerColor = { 1.0f, 162.0f/255.0f, 0.0f, 0.7f };

        DecomposedPosition startposition = {0};
        startposition.tileareaX = 0;
        startposition.tileareaY = 0;
        startposition.tileX     = 2;
        startposition.tileY     = 2;
        startposition.tileRelative = vec2( tilemap->tileInMeters * 0.5f, tilemap->tileInMeters * 0.5f );
        
        player->playerPos       = composePosition( startposition );
        player->size.x          = 0.9f * tilemap->tileInMeters;
        player->size.y          = 0.9f * tilemap->tileInMeters;
        player->acceleration    = 50.0f; // in meters per second squared -> m/s^2
        player->velocityVector  = vec2(0.0f, 0.0f);
        player->color           = playerColor;

        memory->isInitialized = true;
        state->loading        = false;
        state->fullscreen     = false;
    }

    updatePlayer( input, player, tilemap, state->dt );

    drawBackground( buffer );
    drawWorld( buffer, tilemap, player );
    drawPlayer( buffer, player, tilemap );
}
