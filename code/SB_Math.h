#ifndef SB_MATH_H
#define SB_MATH_H

#include "SB_Application.h"

struct v2
{
    union
    {
        struct { f32 x; f32 y; };
        f32 data[2]; 
    };
};

v2 operator+( v2 a, v2 b )
{
    v2 result = {0};
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

v2 operator-( v2 a )
{
    v2 result = {0};
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

v2 operator-( v2 a, v2 b )
{
    v2 result = {0};
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

v2 operator*( f32 x, v2 v )
{
    v2 result = {0};
    result.x = v.x * x;
    result.y = v.y * x;
    return result;
}

#endif//SB_MATH_H