#ifndef SB_INTRINSICS_H
#define SB_INTRINSICS_H

#include "SB_Application.h"
#include <math.h>

void initMath() {
    srand( 10 );
}

inline s32 roundToS32( f32 value )
{
    return (s32)(value + 0.5f);
}

inline s32 truncateToS32( f32 value )
{
    return (s32)value;
}

inline s32 floorToS32( f32 value )
{
    return (s32)floorf( value );
}

inline s32 ceilToS32( f32 value )
{
    return (s32)ceilf( value );
}

inline u32 randomNumber()
{
    return rand();
}

inline f32 squareRoot( f32 x )
{
    return sqrtf( x );
}

inline f32 square( f32 x )
{
    return x * x;
}

inline f32 arcusTanges2( f32 x, f32 y )
{
    return atan2f( x, y );
}

inline f32 arcusCosine( f32 x )
{
    return acosf( x );
}

inline f32 sinus( f32 x )
{
    return sinf( x );
}

inline f32 cosinus( f32 x )
{
    return cosf( x );
}

inline f32 maximum( f32 x, f32 y )
{
    return fmax( x, y );
}

#endif//SB_INTRINSICS_H
