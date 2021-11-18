#include "SB_Entity.h"

void drawPlayer( RenderBuffer* buffer, entity* player )
{

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