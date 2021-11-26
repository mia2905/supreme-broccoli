#ifndef SB_MATH_H
#define SB_MATH_H

#include "SB_Application.h"
#include "SB_Intrinsics.h"

#define PI 3.14159265359
#define PI_2 PI/2
#define PI2  PI * 2.0

union v2
{
    struct { f32 x; f32 y; };
    f32 data[2]; 

    f32 length();
    f32 angle();
    f32 dot( v2 b );
    f32 angleBetween( v2 b );
    void normalize();
};

v2 vec2( f32 x, f32 y )
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

v2 operator-( v2 a, f32 b )
{
    v2 result = a;
    result.x -= b;
    result.y -= b;
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

f32 v2::angleBetween( v2 b )
{
    f32 angle = arcusCosine(this->dot(b));
    
    return angle;
}

f32 toDegrees( f32 radians )
{
    return (180.0f / PI) * radians;
}

f32 toRadians( f32 degrees )
{
    return (PI / 180.0f) * degrees;
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

struct rect
{
    v2 min;
    v2 max;
};

rect squareWithCenter( v2 center, f32 radius)
{
    rect result;
    result.min = center - radius;
    result.max = center + radius;
    return result;
}

rect rectWithCenter( v2 center, v2 size )
{
    rect result;
    result.min.x = center.x - size.x/2.0f;
    result.min.y = center.y - size.y/2.0f;

    result.max.x = result.min.x + size.x;
    result.max.y = result.min.y + size.y;

    return result;
}

rect square2WithCorner( v2 corner, f32 dim )
{
    rect result;
    result.min = corner;
    result.max = corner + dim;
    return result;
}

rect rectWithCorner( v2 corner, v2 size );

bool isInsideRect( rect rect, v2 value )
{
    if( value.x < rect.min.x ) return false;
    if( value.x > rect.max.x ) return false;
    if( value.y < rect.min.y ) return false;
    if( value.y > rect.max.y ) return false;

    return true;
}

#endif//SB_MATH_H