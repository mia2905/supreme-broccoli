#include "SB_Entity.h"
#include "SB_Render.h"

void drawPlayer( RenderBuffer* buffer, entity* player )
{
    Color color = { 75.0f/255.0f, 75.0f/255.0f, 75.0f/255.0f, 0.9f };

    f32 tileMinx = (player->position.x - player->size.x / 2.0f) * buffer->metersToPixels;
    f32 tileMaxx = (player->position.x + player->size.x / 2.0f) * buffer->metersToPixels;
    f32 tileMiny = (player->position.y - player->size.y / 2.0f) * buffer->metersToPixels;
    f32 tileMaxy = (player->position.y + player->size.y / 2.0f) * buffer->metersToPixels;
    drawRectangle( buffer, vec2(tileMinx, tileMiny), vec2(tileMaxx, tileMaxy), color );
}

void drawMonster( RenderBuffer* buffer, entity* monster )
{

}

void drawWall( RenderBuffer* buffer, entity* wall )
{

}

void drawDoor( RenderBuffer* buffer, entity* door )
{

}

void drawEntities( RenderBuffer* buffer, live_entities* e )
{
    for( u32 i=0; i<e->numberOfEntities; ++i )
    {
        drawEntity( buffer, e->entities[i] );
    }
}

void drawEntity( RenderBuffer* buffer, entity* e )
{
    switch( e->type )
    {
        case PLAYER:
            drawPlayer( buffer, e );
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