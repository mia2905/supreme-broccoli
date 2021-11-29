#include "SB_Entity.h"
#include "SB_Render.h"

void drawPlayer( RenderBuffer* buffer, Entity* player, Camera* camera )
{
    Color color = { 75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.9f };

    // the position of the player is relative to the camera
    v2 cameraP   = camera->absolutePosition;
    v2 absCenter = cameraP + player->position;

    f32 tileMinx = (absCenter.x - player->size.x / 2.0f) * buffer->metersToPixels;
    f32 tileMaxx = (absCenter.x + player->size.x / 2.0f) * buffer->metersToPixels;
    f32 tileMiny = (absCenter.y - player->size.y / 2.0f) * buffer->metersToPixels;
    f32 tileMaxy = (absCenter.y + player->size.y / 2.0f) * buffer->metersToPixels;
    drawRectangle( buffer, vec2(tileMinx, tileMiny), vec2(tileMaxx, tileMaxy), color );
}

void drawMonster( RenderBuffer* buffer, Entity* monster )
{

}

void drawWall( RenderBuffer* buffer, Entity* wall )
{

}

void drawDoor( RenderBuffer* buffer, Entity* door )
{

}

void drawEntities( RenderBuffer* buffer, live_entities* e, Camera* camera )
{
    for( u32 i=0; i<e->numberOfEntities; ++i )
    {
        drawEntity( buffer, e->entities[i], camera );
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
            drawMonster( buffer, e );
            break;

        case WALL:
            drawWall( buffer, e );
            break;

        case DOOR:
            drawDoor( buffer, e );
            break;

        default:
            PrintError( "unsupported entity type" );
    }
}