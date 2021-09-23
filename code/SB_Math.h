#ifndef SB_MATH_H
#define SB_MATH_H

#include "SB_Application.h"
#include "SB_Intrinsics.h"

#define PI 3.14159265359
#define PI_2 PI/2

union v2
{
    struct { f32 x; f32 y; };
    f32 data[2]; 

    f32 length();
    f32 angle();
    f32 dot( v2 b );
    f32 angelBetween( v2 b );
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

v2 operator*( f32 x, v2 a )
{
    v2 result = a;
    result.x = result.x * x;
    result.y = result.y * x;
    
    return result;
}

v2 operator+( v2 a, v2 b )
{
    v2 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

v2 operator+( v2 a, f32 x )
{
    v2 result = a;
    result.x = a.x + x;
    result.y = a.y + x;

    return result;
}

v2 operator+( f32 x, v2 a )
{
    v2 result = a;
    result.x = a.x + x;
    result.y = a.y + x;

    return result;
}

v2 operator-( v2 a, v2 b )
{
    v2 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

f32 dot( v2 a, v2 b )
{
    f32 result;

    result = a.x*b.x + a.y*b.y;

    return result;
}

f32 v2::length()
{
    f32 l = sqrtf( square(x) + square(y) );
    return l;
}

f32 v2::angle()
{
    f32 angle = arcusTanges2( this->x, this->y );
    return angle;
}

f32 v2::dot( v2 b )
{
    f32 result;

    result = this->x * b.x + this->y * b.y;

    return result;
}

f32 v2::angelBetween( v2 b )
{
    f32 cosine = (this->dot(b)) / (this->length() * b.length() );
    f32 angle  = cosine * PI_2;

    return angle;
}

void v2::normalize()
{
    if( this->x == 0.0f && this->y == 0.0f )
    {
        return;
    }

    f32 magnitude = this->length();
    this->x = this->x / magnitude;
    this->y = this->y / magnitude;
}

#endif//SB_MATH_H