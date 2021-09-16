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

    v2 operator*=( f32 x );
    v2 operator*( f32 x );
};

v2 V2( f32 x, f32 y )
{
    v2 result;
    result.x = x;
    result.y = y;
    return result;
}

v2 v2::operator*= ( f32 x )
{
    this->x *= x;
    this->y *= y;
    return *this;
}

v2 v2::operator* ( f32 x )
{
    v2 result = *this;
    result.x *= x;
    result.y *= y;
    return result;
}

v2 operator+( v2 a, v2 b )
{
    v2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

v2 operator-( v2 a )
{
    v2 result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

v2 operator-( v2 a, v2 b )
{
    v2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

#endif//SB_MATH_H