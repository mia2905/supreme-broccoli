#ifndef SB_ENTITY_H
#define SB_ENTITY_H

#include "SB_Application.h"
#include "SB_Math.h"

enum entity_type
{
    PLAYER,
    MONSTER,
    WALL,
    DOOR
};

struct entity
{
    v2          size;
    v2          velocity;
    v2          position; // center point
    entity_type type;
};

void drawEntity( RenderBuffer* buffer, entity* e );

#endif//SB_ENTITY_H