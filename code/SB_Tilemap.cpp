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

    newPos.tileareaX    = pos.unifiedPositionX >> TILEAREA_SHIFT;
    newPos.tileareaY    = pos.unifiedPositionY >> TILEAREA_SHIFT;

    newPos.tileX        = pos.unifiedPositionX & TILE_MASK;
    newPos.tileY        = pos.unifiedPositionY & TILE_MASK;

    newPos.tileRelative = pos.tileRelative;

    return newPos;
}

GeneralizedPosition composePosition( DecomposedPosition pos )
{
    GeneralizedPosition result;

    result.unifiedPositionX = pos.tileareaX << TILEAREA_SHIFT;
    result.unifiedPositionY = pos.tileareaY << TILEAREA_SHIFT;
    result.unifiedPositionX = result.unifiedPositionX | pos.tileX;
    result.unifiedPositionY = result.unifiedPositionY | pos.tileY;
    result.tileRelative     = pos.tileRelative;

    return result;
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

GeneralizedPosition buildNewPosition( DecomposedPosition oldPosition, v2 movement, TileMap* tilemap )
{
    GeneralizedPosition result;
    DecomposedPosition  p = oldPosition;
    // 1. calculate new tile x and y coords in the tile area along with a new tilerelative offset in meters
    v2 tempTileRelative = oldPosition.tileRelative + movement;
    f32 x               = tempTileRelative.x;
    f32 y               = tempTileRelative.y;
    u32 tileX           = oldPosition.tileX;
    u32 tileY           = oldPosition.tileY;
    f32 tileSize        = tilemap->tileInMeters;
    p.tileRelative      = tempTileRelative;

    if( x >= tileSize )
    {
        p.tileX         += 1;
        p.tileRelative.x = x - tileSize;
    }
    
    if( x < 0 ) 
    {
        p.tileX         -= 1;
        p.tileRelative.x = tileSize - x;
    }

    if( y >= tileSize )
    {
        p.tileY         += 1;
        p.tileRelative.y = y - tileSize;
    }

    if( y < 0 )
    {
        p.tileY         -= 1;
        p.tileRelative.y = tileSize - y;
    }

    // 2. calculate the new tile x and y and update tilemap x and y
    p.tileareaX = p.tileareaX + generalizeTileIndex( (s32*)&p.tileX, tilemap->tileCountX );
    p.tileareaY = p.tileareaY + generalizeTileIndex( (s32*)&p.tileY, tilemap->tileCountY );

    result = composePosition( p );
    return result;;
}

u32 getTileValue( TileMap* tilemap, TileArea* area, u32 tileX, u32 tileY )
{
    Assert( tilemap != nullptr );

    return area->tiles[tileY * tilemap->tileCountX + tileX];
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
