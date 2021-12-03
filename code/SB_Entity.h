#ifndef SB_ENTITY_H
#define SB_ENTITY_H

#include "SB_Application.h"
#include "SB_Tilemap.h"
#include "SB_Math.h"
#include "SB_Render.h"

#define MAX_ENTITIES 1000

enum entity_type
{
    PLAYER,
    MONSTER,
    WALL,
    DOOR
};

struct Entity
{
    v2          size;
    v2          velocity;
    v2          position; // center point relative to the camera position

    GeneralizedPosition g_position;

    entity_type type;
    bool        moving;

    Color       color;
};

struct live_entities
{
    Entity* entities[MAX_ENTITIES];
    u32     numberOfEntities;
};

struct offline_entities
{
    Entity* entities[MAX_ENTITIES];
    u32     numberOfEntities;
};

void addWall( MemoryPool*    appMemory, 
              live_entities* entities, 
              TileMap*       tilemap, 
              v2             cameraPosition, 
              u32 tileX,     u32 tileY, 
              u32 tileareaX, u32 tileareaY );

void drawEntities( RenderBuffer* buffer, live_entities* entities, Camera* camera );
void drawEntity( RenderBuffer* buffer, Entity* e, Camera* camera );

#endif//SB_ENTITY_H