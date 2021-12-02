#include "SB_Entity.h"
#include "SB_Render.h"
#include "SB_Memory.h"

void addWall( MemoryPool*    appMemory, 
              live_entities* entities, 
              TileMap*       tilemap,
              v2             cameraPosition,
              u32 tileX,     u32 tileY, 
              u32 tileareaX, u32 tileareaY )
{
    Entity* wall = PushStruct( appMemory, Entity );

    wall->size       = vec2( tilemap->tileInMeters, tilemap->tileInMeters );
    wall->type       = WALL;
    wall->velocity   = vec2(0, 0);
    wall->color      = { 75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.5f };
    
    DecomposedPosition d = {0};
    d.tileX        = tileX;
    d.tileY        = tileY;
    d.tileareaX    = tileareaX;
    d.tileareaY    = tileareaY;
    d.tileRelative = wall->size * 0.5f;

    GeneralizedPosition g = composePosition( d );
    v2 wallAbsPos    = toAbsolutePosition( g, tilemap->tileInMeters, tilemap->tileCountX, tilemap->tileCountY );
    v2 wallPosition  = wallAbsPos - cameraPosition;

    wall->g_position = g;
    wall->position   = wallPosition;
    wall->moving     = false;

    entities->entities[entities->numberOfEntities++] = wall;
}

void drawPlayer( RenderBuffer* buffer, Entity* player, Camera* camera )
{
    // the position of the player is relative to the camera
    v2 cameraP   = camera->absolutePosition;
    v2 absCenter = cameraP + player->position;

    f32 tileMinx = (absCenter.x - player->size.x / 2.0f) * buffer->metersToPixels;
    f32 tileMaxx = (absCenter.x + player->size.x / 2.0f) * buffer->metersToPixels;
    f32 tileMiny = (absCenter.y - player->size.y / 2.0f) * buffer->metersToPixels;
    f32 tileMaxy = (absCenter.y + player->size.y / 2.0f) * buffer->metersToPixels;
    
    drawRectangle( buffer, vec2(tileMinx, tileMiny), vec2(tileMaxx, tileMaxy), player->color );
}

void drawMonster( RenderBuffer* buffer, Entity* monster, Camera* camera )
{

}

void drawWall( RenderBuffer* buffer, Entity* wall, Camera* camera )
{
    Color color = { 75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.5f };

    // the position of the player is relative to the camera
    v2 cameraP   = camera->absolutePosition;
    v2 absCenter = cameraP + wall->position;

    f32 tileMinx = (absCenter.x - wall->size.x / 2.0f) * buffer->metersToPixels;
    f32 tileMaxx = (absCenter.x + wall->size.x / 2.0f) * buffer->metersToPixels;
    f32 tileMiny = (absCenter.y - wall->size.y / 2.0f) * buffer->metersToPixels;
    f32 tileMaxy = (absCenter.y + wall->size.y / 2.0f) * buffer->metersToPixels;
    
    drawRectangle( buffer, vec2(tileMinx, tileMiny), vec2(tileMaxx, tileMaxy), wall->color );
}

void drawDoor( RenderBuffer* buffer, Entity* door, Camera* camera )
{

}

void drawEntities( RenderBuffer* buffer, live_entities* e, Camera* camera )
{
    for( u32 i=0; i<e->numberOfEntities; ++i )
    {
        if( isInsideRect( camera->screen, e->entities[i]->position ) )
        {
            drawEntity( buffer, e->entities[i], camera );
        }
    }
}

void drawEntity( RenderBuffer* buffer, Entity* e, Camera* camera )
{
    switch( e->type )
    {
        case PLAYER:
            drawPlayer( buffer, e, camera );
            break;

        case MONSTER:
            drawMonster( buffer, e, camera );
            break;

        case WALL:
            drawWall( buffer, e, camera );
            break;

        case DOOR:
            drawDoor( buffer, e, camera );
            break;

        default:
            PrintError( "unsupported entity type" );
    }
}