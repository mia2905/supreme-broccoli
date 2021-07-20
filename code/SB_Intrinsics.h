#ifndef SB_INTRINSICS_H
#define SB_INTRINSICS_H

#include "SB_Application.h"
#include <math.h>

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

#endif//SB_INTRINSICS_H
