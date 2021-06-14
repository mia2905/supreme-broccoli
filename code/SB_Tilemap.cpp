#include "SB_Application.h"
#include "SB_Tilemap.h"


TileArea* getTileArea( TileMap* tilemap, u32 x, u32 y )
{
    u32 width      = tilemap->tileareaCountX;
    TileArea* area = nullptr;

    if( (x >= 0) && (y < tilemap->tileareaCountX) &&
        (x >= 0) && (y < tilemap->tileareaCountY) )
    {
        area = &tilemap->tileAreas[y * width + x];
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

s32 generalizeCoords( f32* value, s32 pixelSize )
{
    s32 result = 0;
    f32 newX = *value / pixelSize;
    if( newX < 0 ) 
    {
        *value = pixelSize - *value;
        result = -1;
    }
    if( newX >= 1.0 )
    {
        *value = *value - pixelSize;
        result = 1;
    }

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

GeneralizedPosition getGeneralizedPosition( TileMap* tilemap, GeneralizedPosition pos )
{
    GeneralizedPosition result = pos;
    DecomposedPosition  newPos = decomposePosition( pos );
    
    // 1. calculate the new tile relative x and y
    newPos.tileX = newPos.tileX + generalizeCoords( &result.x, tilemap->tileInMeters );
    newPos.tileY = newPos.tileY + generalizeCoords( &result.y, tilemap->tileInMeters );

    // 2. calculate the new tile x and y and update tilemap x and y
    newPos.tileareaX = newPos.tileareaX + generalizeTileIndex( (s32*)&newPos.tileX, tilemap->tileCountX );
    newPos.tileareaY = newPos.tileareaY + generalizeTileIndex( (s32*)&newPos.tileY, tilemap->tileCountY );

    composePosition( newPos, &result );

    return result;
}

bool isMoveAllowed( GeneralizedPosition newPos, TileMap* tilemap )
{
    bool allowed = false;

    DecomposedPosition pos = decomposePosition( newPos );
    TileArea* area = getTileArea( tilemap, pos.tileareaX, pos.tileareaY );
    
    if( area )
    {
        if( (pos.tileX >= 0) && (pos.tileX < tilemap->tileCountX) &&
            (pos.tileY >= 0) && (pos.tileY < tilemap->tileCountY) )
        {
            allowed = ( area->tiles[pos.tileY * tilemap->tileCountX + pos.tileX] == 0 ); 
        }
    }
    
    return allowed;
}

void buildTileArea( TileMap* map, u32 areaX, u32 areaY )
{
    TileArea* area = getTileArea( map, areaX, areaY );
    Assert( (area != nullptr) );

    u32* tile = area->tiles;

    for( u32 row=0; row<TILEMAP_Y; ++row )
    {
        for( u32 col=0; col<TILEMAP_X; ++col )
        {
            *tile = 0;

            if( row == 0 )                *tile = 1;
            if( row == (TILEMAP_Y - 1) )  *tile = 1;
            if( col == 0 )                *tile = 1;
            if( col == (TILEMAP_X - 1 ) ) *tile = 1;
            if( row == (TILEMAP_Y / 2) )  *tile = 0;
            if( col == (TILEMAP_X / 2) )  *tile = 0;
            
            tile++;
        }
    }
}
