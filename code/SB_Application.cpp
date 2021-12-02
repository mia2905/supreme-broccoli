#include "SB_Application.h"
#include "SB_Intrinsics.h"
#include "SB_Math.h"
#include "SB_Render.cpp"
#include "SB_Memory.cpp"
#include "SB_Tilemap.cpp"
#include "SB_Sound.cpp"
#include "SB_Entity.cpp"

void buildWorld( MemoryPool* pool, TileMap* map, Camera* camera, live_entities* entities )
{
    u32 x = 0;
    u32 y = 0;

    for( u32 i=0; i<NR_OF_TILEAREAS; ++i )
    {
        TileArea* area = nullptr;

        area = getTileArea( map, x, y );
        area->tiles = PushArray( pool, map->tileCountX * map->tileCountY, u32 );
    
        u32* tile = area->tiles;
        f32 absoluteTileAreaWidth  = map->tileCountX * map->tileInMeters;
        f32 absoluteTileAreaHeight = map->tileCountY * map->tileInMeters;

        for( u32 row=0; row<map->tileCountY; ++row )
        {
            for( u32 col=0; col<map->tileCountX; ++col )
            {
                *tile = 0;

                if( row == 0 ) *tile = 1;
                if( col == 0 ) *tile = 1;
                if( row == (map->tileCountY - 1) ) *tile = 1;
                if( col == (map->tileCountX - 1) ) *tile = 1;

                v2 tileAreaCamera = camera->absolutePosition;

                if( *tile == 1 && col != map->tileCountX / 2 && row != map->tileCountY / 2 )
                {
                    addWall( pool, entities, map, tileAreaCamera, col, row, x, y );
                }

                tile++;
            }
        }

        // update tilearea coords
        if( x < NR_OF_TILEAREAS / 2 )
        {
            x++;
        }
        else
        {
            if( x == NR_OF_TILEAREAS / 2 )
            {
                x = 0;
            }
            else
            {
                x++;
            }
            y = 1;
        }
    }
}

void drawWorld( RenderBuffer* buffer, TileMap* world, Camera* camera )
{
    Color backgroundColor = { 200.0f/255.0f, 200.0f/255.0f, 200.0f/255.0f, 1.0f };
    drawRectangle( buffer, vec2(0, 0), vec2(buffer->width, buffer->height), backgroundColor );
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

void moveEntities( UserInput*     input, 
                   live_entities* entities, 
                   TileMap*       tilemap, 
                   f32            dt,
                   f32            acceleration,
                   v2             direction )
{
    static v2 north = vec2(  0.0f,  1.0f );
    static v2 south = vec2(  0.0f, -1.0f );
    static v2 east  = vec2(  1.0f,  0.0f );
    static v2 west  = vec2( -1.0f,  0.0f );
    
    for( u32 i=0; i<entities->numberOfEntities; ++i )
    {
        Entity* e = entities->entities[i];
        
        if( e->moving )
        {
            v2 accelerationVector = direction * acceleration - (4.0f * e->velocity);
            v2 velocity           = ((2.0f * accelerationVector) * dt) + e->velocity;
            
            // equations of motion:
            // p' = 1/2at^2 + vt + p -> acceleration based position
            // v' = 2at + v          -> acceleration based velocity ( 1st derivative of the position  )
            // a' = a                -> acceleration                ( 2nd derivative of the velocity )
            v2 movement = ((0.5f * accelerationVector) * square( dt )) + (velocity * dt);

            // build a search space with all entities inside of defined area (1.5 * tilesize)
            f32     searchDistance    = tilemap->tileInMeters;
            Entity* searchEntities[8] = {0};
            u32     searchIndex       = 0;
            Color   c                 = { 75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.5f };

            for( u32 s=0; s<entities->numberOfEntities && searchIndex < 8; ++s )
            {
                Entity* t = entities->entities[s];
                if( t != e )
                {
                    v2 a = e->position;
                    v2 b = t->position;

                    if( distanceBetween(a,b) <= searchDistance )
                    {
                        searchEntities[searchIndex++] = t;
                        t->color = { 1.0f, 0.0f, 0.0f, 0.5f };
                    }
                    else
                    {
                        t->color = c;
                    }
                }
            }

            v2  playerSize    = e->size;
            f32 timeRemaining = 1.0f;
            v2 wallNormal     = {0};

            for( u32 iteration=0; (iteration<4) && (timeRemaining > 0.0f); ++iteration)
            {
                f32 tMin = 1.0f;

                for( u32 s=0; s<searchIndex; s++ )
                {
                    Entity* t = searchEntities[s];

                    rect entityBounds = rectWithCenter( t->position, t->size * 0.5 );
                    v2 minCorner  = entityBounds.min - (playerSize * 0.5f);
                    v2 maxCorner  = entityBounds.max + (playerSize * 0.5f);
                    v2 p0         = e->position;
                    
                    if( wallCollision( maxCorner.y, movement.y, movement.x, p0.y, p0.x, &tMin, minCorner.x, maxCorner.x ) )
                    {
                        Print( "COLLISION NORTH\n" );
                        wallNormal = north;
                    }
                    if( wallCollision( minCorner.y, movement.y, movement.x, p0.y, p0.x, &tMin, minCorner.x, maxCorner.x ) )
                    {
                        Print( "COLLISION SOUTH\n" );
                        wallNormal = south;
                    }
                    if( wallCollision( minCorner.x, movement.x, movement.y, p0.x, p0.y, &tMin, minCorner.y, maxCorner.y ) )
                    {
                        Print( "COLLISION WEST\n" );
                        wallNormal = west;
                    }
                    if( wallCollision( maxCorner.x, movement.x, movement.y, p0.x, p0.y, &tMin, minCorner.y, maxCorner.y ) )
                    {
                        Print( "COLLISION EAST\n" );
                        wallNormal = east;
                    }
                }

                e->g_position = updatePosition( e->g_position, movement * tMin, tilemap );

                // update the movement
                movement = movement - dot(movement, wallNormal)*wallNormal;
            
                // update the velocity
                velocity = velocity - dot(velocity, wallNormal)*wallNormal;
                e->velocity = velocity;            
                e->position = e->position + movement * tMin;
                
                // update time to collision
                timeRemaining -= timeRemaining * tMin;    
            }
        }
    }
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
    Camera*        camera   = state->camera;

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
        Entity* e = entities->entities[i];

        if( e->moving )
        {
            moveEntities( input, entities, tilemap, state->dt, acceleration, direction );
        }
    }

    // check if the camera needs to update    
    Entity* player = camera->followingEntity;
    
    // 1. construct the screen rect
    // 2. check if the camera following entity has left the rect
    // 3. if it has
    //    a) update all entities to be relative to the new camera position
    //    b) update the camera tile position

    u32 newTileAreaX = 0;
    u32 newTileAreaY = 0;

    if( !isInsideRect( camera->screen, player->position ) )
    {
        for( u32 i=0; i<entities->numberOfEntities; ++i )
        {
            Entity* e = entities->entities[i];
            
            DecomposedPosition p = decomposePosition( player->g_position );
            DecomposedPosition c = decomposePosition( camera->tilePosition );

            // camera change vector -> depends on the new tilearea
            v2 a   = vec2( (f32)(p.tileareaX - c.tileareaX), 
                           (f32)(p.tileareaY - c.tileareaY) ) *
                     vec2( (f32)(tilemap->tileCountX * tilemap->tileInMeters) ,
                           (f32)(tilemap->tileCountY * tilemap->tileInMeters) );

            // map the entity position to the new camera
            e->position = e->position - a;

            if( e == player )
            {
                newTileAreaX = p.tileareaX;
                newTileAreaY = p.tileareaY;
            }
        }

        // update the tilearea aka screen to be shown
        camera->tilePosition = player->g_position;
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
        state->tilemap                       = PushStruct( appMemory, TileMap );
        state->liveEntities.entities[0]      = PushStruct( appMemory, Entity );
        state->liveEntities.numberOfEntities = 1;
        tilemap = state->tilemap;
        player  = state->liveEntities.entities[0];

        state->tilemap->tileCountX         = TILES_PER_AREA_X;
        state->tilemap->tileCountY         = TILES_PER_AREA_Y;
        state->tilemap->tileInMeters       = 2.0f;
        state->tilemap->tileInPixels       = TILE_SIZE;
        state->tilemap->tileAreas          = PushArray( appMemory, 
                                                 NR_OF_TILEAREAS * NR_OF_TILEAREAS,
                                                 TileArea );

        buffer->metersToPixels  = (f32)((f32)state->tilemap->tileInPixels/state->tilemap->tileInMeters);

        // create the camera
        state->camera                      = PushStruct( appMemory, Camera );
        camera = state->camera;
        camera->absolutePosition    = vec2( tilemap->tileCountX * tilemap->tileInMeters,
                                            tilemap->tileCountY * tilemap->tileInMeters ) * 0.5f;

        u32 cameraTileX = (u32)(camera->absolutePosition.x / tilemap->tileInMeters);
        u32 cameraTileY = (u32)(camera->absolutePosition.y / tilemap->tileInMeters);
        camera->tilePosition        = buildPosition( cameraTileX,
                                                     cameraTileY,
                                                     (u32)0, // tilearea x 
                                                     (u32)0, // tilearea y
                                                     vec2( camera->absolutePosition.x - (cameraTileX * tilemap->tileInMeters),
                                                           camera->absolutePosition.y - (cameraTileY * tilemap->tileInMeters)) );
        camera->absolutePosition    = toAbsolutePosition( camera->tilePosition, tilemap->tileInMeters, tilemap->tileCountX, tilemap->tileCountY );                                                            
        camera->followingEntity     = player;  

        f32  dimX   = (tilemap->tileCountX * tilemap->tileInMeters) / 2.0f;
        f32  dimY   = (tilemap->tileCountY * tilemap->tileInMeters) / 2.0f;
        camera->screen = rectWithCenter( vec2( 0.0f, 0.0f ), vec2( dimX, dimY ) );

                                                         

        buildWorld( appMemory, tilemap, camera, &state->liveEntities );             

        const char* playerfile   = "./art/player.png";
        const char* mp3File      = "./assets/music/datahop.mp3";

        state->mp3Samples        = state->services.loadMp3( appMemory, mp3File );
        
        // the player starts at the camera location
        player->position        = vec2( 0.0f, 0.0f );
        player->g_position      = camera->tilePosition;
        player->size            = vec2( 0.9f * tilemap->tileInMeters, 0.9f * tilemap->tileInMeters );
        player->velocity        = vec2(0.0f, 0.0f);
        player->type            = PLAYER;
        player->moving          = true;
        player->color           = { 75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.9f };
        
        memory->isInitialized = true;
        state->loading        = false;
        state->fullscreen     = false;
    }

    processInput( input, state );
    
    drawWorld( buffer, tilemap, camera );
    drawEntities( buffer, &state->liveEntities, camera );
}
