#ifndef SB_ENTITY_H
#define SB_ENTITY_H

#include "SB_Application.h"
#include "SB_Tilemap.h"
#include "SB_Math.h"

#define MAX_ENTITIES 1000

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
    v2          position; // center point relative to the bottom left corner of the screen

    GeneralizedPosition g_position;

    entity_type type;
};

struct live_entities
{
    entity* entities[MAX_ENTITIES];
    u32     numberOfEntities;
};

struct offline_entities
{
    entity* entities[MAX_ENTITIES];
    u32     numberOfEntities;
};

void drawEntities( RenderBuffer* buffer, live_entities* entities );
void drawEntity( RenderBuffer* buffer, entity* e );

#endif//SB_ENTITY_H