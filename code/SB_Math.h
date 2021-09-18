#ifndef SB_MATH_H
#define SB_MATH_H

#include "SB_Application.h"
#include "SB_Intrinsics.h"

union v2
{
    struct { f32 x; f32 y; };
    f32 data[2]; 

    f32 length();
    f32 angle();
    void normalize();
};

v2 V2( f32 x, f32 y )
{
    v2 result;
    result.x = x;
    result.y = y;
    return result;
}

v2 operator*( v2 a, f32 x )
{
    v2 result = a;
    result.x *= x;
    result.y *= x;
    return result;
}

v2 operator+( v2 a, v2 b )
{
    v2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

v2 operator-( v2 a, v2 b )
{
    v2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

f32 v2::length()
{
    f32 l = sqrtf( this->x * this->x + this->y * this->y );
    return l;
}

f32 v2::angle()
{
    f32 angle = arcusTanges2( this->x, this->y );
    return angle;
}

void v2::normalize()
{
    f32 magnitude = this->length();
    this->x = this->x / magnitude;
    this->y = this->y / magnitude;
}



#endif//SB_MATH_H