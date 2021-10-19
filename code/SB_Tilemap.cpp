#include "SB_Application.h"
#include "SB_Tilemap.h"
#include "SB_Intrinsics.h"


TileArea* getTileArea( TileMap* tilemap, s32 x, s32 y )
{
    TileArea* area = NULL;

    if( x >= 0 && y >= 0 ) 
    {
        area = &tilemap->tileAreas[y * NR_OF_TILEAREAS + x];
    }
    
    return area;
}

DecomposedPosition decomposePosition( GeneralizedPosition pos )
{
    DecomposedPosition newPos = {0};

    newPos.tileareaX = pos.unifiedPositionX >> TILEAREA_SHIFT;
    newPos.tileareaY = pos.unifiedPositionY >> TILEAREA_SHIFT;

    newPos.tileX     = pos.unifiedPositionX & TILE_MASK;
    newPos.tileY     = pos.unifiedPositionY & TILE_MASK;

    return newPos;
}

void composePosition( DecomposedPosition pos, GeneralizedPosition* newPos )
{
    newPos->unifiedPositionX = pos.tileareaX << TILEAREA_SHIFT;
    newPos->unifiedPositionY = pos.tileareaY << TILEAREA_SHIFT;
    newPos->unifiedPositionX = newPos->unifiedPositionX | pos.tileX;
    newPos->unifiedPositionY = newPos->unifiedPositionY | pos.tileY;
}

s32 generalizeTileIndex( s32* tileIndex, s32 tileCount )
{
    s32 result = 0;
    s32 index = *tileIndex;

    if( index < 0 )
    {
        *tileIndex = tileCount + index;
        result = -1;
    }
    if( index >= tileCount )
    {
        *tileIndex = index - tileCount;
        result = 1;
    }

    return result;
}

v2 getNewTilesAndUpdateOffset( v2* tileRelativePosition /* in meters */, f32 tileSize /* in meters */ )
{
    v2 result;

    f32 x = tileRelativePosition->x;
    f32 y = tileRelativePosition->y;

    if( x >= tileSize ) 
    {
        result.x = 1;
        tileRelativePosition->x = x - tileSize;
    }

    if( x < 0) 
    {
        result.x = -1;
        tileRelativePosition->x = tileSize - x;
    }

    if( y >= tileSize ) 
    {
        result.y = 1;
        tileRelativePosition->y = y - tileSize;
    }

    if( y < 0) 
    {
        result.y = -1;
        tileRelativePosition->y = tileSize + y;
    }

    return result;
}

GeneralizedPosition getGeneralizedPosition( TileMap* tilemap, GeneralizedPosition pos )
{
    GeneralizedPosition result = pos;
    DecomposedPosition  newPos = decomposePosition( pos );
    
    // 1. calculate the new tiles in x and y
    v2 tileOffset = getNewTilesAndUpdateOffset( &result.tileRelative, tilemap->tileInMeters );
    newPos.tileX = newPos.tileX + tileOffset.x;
    newPos.tileY = newPos.tileY + tileOffset.y;

    // 2. calculate the new tile x and y and update tilemap x and y
    newPos.tileareaX = newPos.tileareaX + generalizeTileIndex( (s32*)&newPos.tileX, tilemap->tileCountX );
    newPos.tileareaY = newPos.tileareaY + generalizeTileIndex( (s32*)&newPos.tileY, tilemap->tileCountY );

    composePosition( newPos, &result );

    return result;
}

u32 getTileValue( TileMap* tilemap, TileArea* area, u32 tileX, u32 tileY )
{
    Assert( tilemap != nullptr );

    return area->tiles[tileY * tilemap->tileCountX + tileX];
}

bool isMoveAllowed( GeneralizedPosition newPos, TileMap* tilemap )
{
    bool allowed = false;

    DecomposedPosition pos = decomposePosition( newPos );
    TileArea* area = getTileArea( tilemap, pos.tileareaX, pos.tileareaY );
    
    if( area->tiles )
    {
        if( (pos.tileX >= 0) && (pos.tileX < tilemap->tileCountX) &&
            (pos.tileY >= 0) && (pos.tileY < tilemap->tileCountY) )
        {
            allowed = ( area->tiles[pos.tileY * tilemap->tileCountX + pos.tileX] == 0 ); 
        }
    }
    
    return allowed;
}

/***********************************
 * only called once during startup
 ***********************************/

TileArea* buildTileArea( MemoryPool* pool, TileMap* map, u32 areaX, u32 areaY )
{
    TileArea* area = getTileArea( map, areaX, areaY );
    area->tiles = PushArray( pool, map->tileCountX * map->tileCountY, u32 );
    
    u32* tile = area->tiles;

    for( u32 row=0; row<TILES_PER_AREA_Y; ++row )
    {
        for( u32 col=0; col<TILES_PER_AREA_X; ++col )
        {
            *tile = 0;

            if( row == 0 )                *tile = 1;
            if( row == (TILES_PER_AREA_Y - 1) )  *tile = 1;
            if( col == 0 )                *tile = 1;
            if( col == (TILES_PER_AREA_X - 1 ) ) *tile = 1;
            
            tile++;
        }
    }

    return area;
}

void setDoor( TileArea* area, DOOR_DIRECTION door )
{
    u32* tiles = area->tiles;

    switch( door )
    {
        case LEFT:   
            tiles[(TILES_PER_AREA_Y / 2 * TILES_PER_AREA_X) + 1] = 0;
            tiles[TILES_PER_AREA_Y / 2 * TILES_PER_AREA_X]       = 0;                 
            break;
        case RIGHT:  
            tiles[TILES_PER_AREA_Y / 2 * TILES_PER_AREA_X + TILES_PER_AREA_X - 2] = 0; 
            tiles[TILES_PER_AREA_Y / 2 * TILES_PER_AREA_X + TILES_PER_AREA_X - 1] = 0; 
            break;
        case TOP:    
            tiles[(TILES_PER_AREA_Y - 1) * TILES_PER_AREA_X - (TILES_PER_AREA_X / 2)] = 0;   
            tiles[TILES_PER_AREA_Y * TILES_PER_AREA_X - (TILES_PER_AREA_X / 2)]       = 0;   
            break;
        case BOTTOM: 
            tiles[TILES_PER_AREA_X + TILES_PER_AREA_X / 2] = 0;
            tiles[TILES_PER_AREA_X / 2]             = 0;                             
            break;
    
        default: break;
    }
}
