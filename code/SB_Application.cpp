#include "SB_Application.h"
#include "SB_Intrinsics.h"
#include "SB_Math.h"
#include "SB_Render.cpp"
#include "SB_Memory.cpp"
#include "SB_Tilemap.cpp"
#include "SB_Sound.cpp"
#include "SB_Entity.cpp"

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

void drawTileArea( RenderBuffer* buffer, TileMap* tilemap, TileArea* area )
{
    Color tilecolor = { 53.0f/255.0f, 51.0f/255.0f, 47.0f/255.0f, 0.8f };

    u32 tileWidth   = tilemap->tileInMeters * buffer->metersToPixels;
    u32 tileHeight  = tilemap->tileInMeters * buffer->metersToPixels;
    u32 tilesPerRow = tilemap->tileCountX;
    u32 rows        = tilemap->tileCountY;

    for( u32 row=0; row<rows; ++row )
    {
        for( u32 col=0; col<tilesPerRow; ++col )
        {
            u32 tile = area->tiles[row * tilemap->tileCountX + col];
            if( tile == 1 )
            {
                u32 minX = ( col  * tileWidth );
                u32 maxX = ( minX + tileWidth );

                // leave a small space between tiles
                minX += 1;
                maxX -= 1;

                u32 minY = row * tileHeight;
                u32 maxY = minY + tileHeight;

                minY += 1;
                maxY -= 1;

                drawRectangle( buffer, vec2(minX, minY), vec2(maxX, maxY), tilecolor );
            }
        }
    }
}

void drawWorld( RenderBuffer* buffer, TileMap* world, Camera* camera )
{
    DecomposedPosition p = decomposePosition( camera->position );

    TileArea* area = getTileArea( world, p.tileareaX, p.tileareaY );
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

void collisionDetection( Entity*  player, 
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

    DecomposedPosition oldPosition; // = decomposePosition( player->playerPos );
    TileArea*          tileArea    = getTileArea( tilemap, oldPosition.tileareaX, oldPosition.tileareaY );
    
    v2 accelerationVector    = direction * acceleration - (4.0f * player->velocity);
    v2 velocity              = ((2.0f * accelerationVector) * dt) + player->velocity;
    v2 playerSize            = player->size;
    player->velocity         = velocity;
    
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
        //player->playerPos = buildNewPosition( oldPosition, movement * t, tilemap );
        
        // update the movement
        movement = movement - dot(movement, wallNormal)*wallNormal;
        
        // update the velocity
        velocity = velocity - dot(velocity, wallNormal)*wallNormal;
        player->velocity = velocity;
        
        // update time to collision
        timeRemaining -= timeRemaining * t;        
    }

    return;
}

void updateEntity( UserInput* input, 
                   Entity*    entity, 
                   TileMap*   tilemap, 
                   f32        dt,
                   f32        acceleration,
                   v2         direction )
{
    v2 accelerationVector    = direction * acceleration - (4.0f * entity->velocity);
    v2 velocity              = ((2.0f * accelerationVector) * dt) + entity->velocity;
    entity->velocity         = velocity;

    // equations of motion:
    // p' = 1/2at^2 + vt + p -> acceleration based position
    // v' = 2at + v          -> acceleration based velocity ( 1st derivative of the position  )
    // a' = a                -> acceleration                ( 2nd derivative of the velocity )
    v2 movement = ((0.5f * accelerationVector) * square( dt )) + (velocity * dt);
    entity->position   = entity->position + movement;
    entity->g_position = updatePosition( entity->g_position, movement, tilemap );
}

void processInput( UserInput* input, ApplicationState* state )
{
    if( input->key_f.isDown )
    {
        state->fullscreen = !state->fullscreen;
        state->services.toggleFullScreen(state->fullscreen);
    }

    if( input->key_p.isDown )
    {
        state->services.playAudio();
    }

    if( input->key_s.isDown )
    {
        state->services.stopAudio();
    }

    live_entities* entities = &state->liveEntities;
    TileMap*       tilemap  = state->tilemap;

    f32 acceleration = input->space.isDown ? 3.0f * 50.0f : 50.0f;
    v2 direction     = vec2(0.0f, 0.0f);
    
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

    direction = normalize( direction );

    for( u32 i=0; i<entities->numberOfEntities; ++i )
    {
        updateEntity( input, 
                      entities->entities[i], 
                      tilemap, 
                      state->dt, 
                      acceleration, 
                      direction );
    }
}

void UpdateAndRender( ApplicationMemory* memory, 
                      RenderBuffer*      buffer,  
                      UserInput*         input )
{
    /**
        ++ ApplicationState ++

        -  MemoryPool*       appMemory      // equal to the start of ApplicationState
        -  PlatformServices  services       // pointer to platform specific implementations
        -  TileMap*          tilemap        // the world
        -  Camera*           camera         // the camera (middle of the screen)
        -  live_entities     liveEntities   // player, walls, monsters - everything which somehow interacts
        -  File*             backgroundMp3
        -  Mp3Buffer*        mp3Samples
        -  bool              loading        // if true platform is not ready yet
        -  bool              fullscreen
        -  f32               dt             // time step this frame
     */
    ApplicationState* state = (ApplicationState*)memory->permanentMemory;
    
    MemoryPool*       appMemory   = state->appMemory;
    TileMap*          tilemap     = state->tilemap;
    Entity*           player      = state->liveEntities.entities[0];
    Camera*           camera      = state->camera;

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
         *  struct ApplicationState
         *  struct MemoryPool
         *  backing store of the MemoryPool
         * 
         *********************/ 

        state->appMemory                   = (MemoryPool*)( state + sizeof( ApplicationState ) );;
        state->appMemory->base             = (u8*)( state->appMemory + sizeof( MemoryPool ) );
        state->appMemory->size             = MegaBytes( 200 );
        state->appMemory->usedBytes        = 0;

        appMemory = state->appMemory;

        // create the tilemap in permanent memory
        state->tilemap                     = PushStruct( appMemory, TileMap );
        state->liveEntities.entities[0]    = PushStruct( appMemory, Entity );
        state->liveEntities.numberOfEntities++;
        tilemap = state->tilemap;
        player  = state->liveEntities.entities[0];

        state->tilemap->tileCountX         = TILES_PER_AREA_X;
        state->tilemap->tileCountY         = TILES_PER_AREA_Y;
        state->tilemap->tileInMeters       = 2.0f;
        state->tilemap->tileInPixels       = TILE_SIZE;
        state->tilemap->tileAreas          = PushArray( appMemory, 
                                                 NR_OF_TILEAREAS * NR_OF_TILEAREAS,
                                                 TileArea );

        // create the camera
        state->camera                      = PushStruct( appMemory, Camera );
        state->camera->position            = buildPosition( (u32)tilemap->tileCountX / 2,
                                                            (u32)tilemap->tileCountY / 2,
                                                            (u32)0,
                                                            (u32)0,
                                                            vec2( tilemap->tileInMeters / 2.0f, tilemap->tileInMeters / 2.0f ) );
        state->camera->followingEntity     = player;                                                            

        camera = state->camera;
                                                 

        buildWorld( appMemory, tilemap );             

        const char* playerfile   = "./art/player.png";
        const char* mp3File      = "./assets/music/datahop.mp3";

        state->mp3Samples        = state->services.loadMp3( appMemory, mp3File );
        
        DecomposedPosition startposition = {0};
        startposition.tileareaX = 0;
        startposition.tileareaY = 0;
        startposition.tileX     = 2;
        startposition.tileY     = 2;
        startposition.tileRelative = vec2( tilemap->tileInMeters * 0.5f, tilemap->tileInMeters * 0.5f );
        
        buffer->metersToPixels  = (f32)((f32)state->tilemap->tileInPixels/state->tilemap->tileInMeters);
        player->position        = vec2( 4.0f * tilemap->tileInMeters, 4.0f * tilemap->tileInMeters );
        player->size            = vec2( 0.9f * tilemap->tileInMeters, 0.9f * tilemap->tileInMeters );
        player->velocity        = vec2(0.0f, 0.0f);
        player->type            = PLAYER;
        
        memory->isInitialized = true;
        state->loading        = false;
        state->fullscreen     = false;
    }

    processInput( input, state );
    
    drawBackground( buffer );
    drawWorld( buffer, tilemap, camera );
    drawEntities( buffer, &state->liveEntities );
}
