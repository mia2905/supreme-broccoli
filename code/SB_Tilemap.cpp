
#include "SB_Tilemap.h"

TileArea* getTileArea( TileMap* tilemap, s32 x, s32 y )
{
    TileArea* area = NULL;

    if( x > NR_OF_TILEAREAS || y > NR_OF_TILEAREAS )
    {
        PrintError( "tile area index out of bounds" );
    }

    if( x >= 0 && y >= 0 ) 
    {
        area = &tilemap->tileAreas[y * NR_OF_TILEAREAS + x];
    }

    if( area == nullptr )
    {
        PrintError( "no valid tile area" );
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

s32 buildNewTileCoord( s32 tile, s32 tileCount )
{
    s32 newTile = tile;

    if( tile < 0 )
    {
        newTile = tileCount + tile;
    }

    if( tile >= tileCount )
    {
        newTile = tile - tileCount;
    }

    if( newTile >= tileCount )
    {
        newTile = newTile % tileCount;
    }

    return newTile;
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
    f32 tileSize = tilemap->tileInMeters;
    f32 x        = oldPosition.tileX * tileSize + oldPosition.tileRelative.x + movement.x;
    f32 y        = oldPosition.tileY * tileSize + oldPosition.tileRelative.y + movement.y;

    p.tileX = x / tileSize;
    p.tileRelative.x = x - (p.tileX * tileSize);

    p.tileY = y / tileSize;
    p.tileRelative.y = y - (p.tileY * tileSize );

    // 2. calculate the new tile x and y and update tilearea x and y
    p.tileareaX = p.tileareaX + generalizeTileIndex( (s32*)&p.tileX, tilemap->tileCountX );
    p.tileareaY = p.tileareaY + generalizeTileIndex( (s32*)&p.tileY, tilemap->tileCountY );

    if( p.tileareaX >= 0 && p.tileareaY >= 0 )
    {
        result = composePosition( p );    
    }
    else
    {
        result = composePosition( oldPosition );
    }
    
    return result;
}

GeneralizedPosition buildPosition( u32 tileX, u32 tileY, u32 areaX, u32 areaY, v2 tileRelative )
{
    DecomposedPosition p = {0};
    p.tileareaX    = areaX;
    p.tileareaY    = areaY;
    p.tileX        = tileX;
    p.tileY        = tileY;
    p.tileRelative = tileRelative;

    return composePosition( p );
}

v2 toAbsolutePosition( GeneralizedPosition p, f32 tileInMeters, u32 tileCountX, u32 tileCountY )
{
    f32 areaWidth  = tileCountX * tileInMeters;
    f32 areaHeight = tileCountY * tileInMeters;

    DecomposedPosition d = decomposePosition( p );
    v2 result = vec2( (d.tileX * tileInMeters) + (d.tileRelative.x), 
                      (d.tileY * tileInMeters) + (d.tileRelative.y));

    result = result + vec2( d.tileareaX * areaWidth, d.tileareaY * areaHeight );
    return result;
}

u32 getTileValue( TileMap* tilemap, TileArea* area, u32 tileX, u32 tileY )
{
    Assert( tilemap != nullptr );

    return area->tiles[tileY * tilemap->tileCountX + tileX];
}

GeneralizedPosition updatePosition( GeneralizedPosition p, v2 movement, TileMap* tilemap )
{
    DecomposedPosition dp = decomposePosition( p );
    return buildNewPosition( dp, movement, tilemap );    
}

/***********************************
 * only called once during startup
 ***********************************/
void printTileArea( TileArea* area, TileMap* tilemap )
{
    u32 tilesX = tilemap->tileCountX;
    u32 tilesY = tilemap->tileCountY;

    for( s32 row = tilesY-1; row >= 0; row-- )
    {
        for( s32 col = 0; col < tilesX; col++ )
        {
            if( getTileValue( tilemap, area, col, row ) == 0 )
            {
                Print( "   " );
            }
            else
            {
                Print( " * " );
            }
        }

        Print( "\n" );
    }
}
