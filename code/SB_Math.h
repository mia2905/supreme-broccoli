#ifndef SB_MATH_H
#define SB_MATH_H

#include "SB_Application.h"
#include "SB_Intrinsics.h"

#define PI 3.14159265359
#define PI_2 PI/2
#define PI2  PI * 2.0

/***********************
 * MATH INTERFACE
 ***********************/
union v2;

v2 vec2( f32 x, f32 y );
v2 operator*( v2 a, f32 x );
v2 operator*( f32 x, v2 a );
v2 operator*( v2 a, v2 b );
v2 operator+( v2 a, v2 b );
v2 operator+( v2 a, f32 x );
v2 operator+( f32 x, v2 a );
v2 operator-( v2 a, v2 b );
v2 operator-( v2 a, f32 x );
v2 operator-( f32 x, v2 a );

f32 dot( v2 a, v2 b );
f32 toDegress( f32 radians );
f32 toRadians( f32 degrees );
f32 length( v2 a );
v2  normalize( v2 a );
f32 angleBetween( v2 a, v2 b );
f32 distanceBetween( v2 a, v2 b );

struct rect;

rect squareWithCenter( v2 center, f32 radius);
rect rectWithCenter( v2 center, v2 size );
rect squareWithCorner( v2 corner, f32 dim );
rect rectWithCorner( v2 corner, v2 size );

bool isInsideRect( rect rect, v2 value );

/***********************
 * MATH IMPLEMENTATION
 ***********************/
union v2
{
    struct { f32 x; f32 y; };
    f32 data[2]; 
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

v2 operator*( v2 a, v2 b )
{
    v2 result = a;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
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
    return a + x;
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

v2 operator-( f32 x, v2 a )
{
    return a - x;
}

f32 dot( v2 a, v2 b )
{
    f32 result;

    result = a.x*b.x + a.y*b.y;

    return result;
}

f32 toDegrees( f32 radians )
{
    return (180.0f / PI) * radians;
}

f32 toRadians( f32 degrees )
{
    return (PI / 180.0f) * degrees;
}

f32 length( v2 a )
{
    f32 l = sqrtf( square(a.x) + square(a.y) );
    return l;
}

v2 normalize( v2 a )
{
    if( a.x == 0.0f && a.y == 0.0f )
    {
        return a;
    }

    f32 magnitude = length(a);

    return vec2( a.x / magnitude, a.y / magnitude );
}

f32 angleBetween( v2 a, v2 b )
{
    f32 angle = arcusCosine(dot( a, b ) );
    
    return angle;
}

f32 distanceBetween( v2 a, v2 b )
{
    f32 result = length( a - b );
    return result;
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

    result.min = center - size;
    result.max = center + size;

    return result;
}

rect squareWithCorner( v2 corner, f32 dim )
{
    rect result;
    result.min = corner;
    result.max = corner + dim;
    return result;
}

rect rectWithCorner( v2 corner, v2 size )
{
    rect result;
    result.min = corner;
    result.max = corner + size;
    return result;
}

bool isInsideRect( rect rect, v2 value )
{
    if( value.x < rect.min.x ) return false;
    if( value.x > rect.max.x ) return false;
    if( value.y < rect.min.y ) return false;
    if( value.y > rect.max.y ) return false;

    return true;
}

#endif//SB_MATH_H