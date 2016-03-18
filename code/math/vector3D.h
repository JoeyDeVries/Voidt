/*******************************************************************
** Copyright (C) 2015-2016 {Joey de Vries} {joey.d.vries@gmail.com}
**
** This code is part of Voidt.
** https://github.com/JoeyDeVries/Voidt
**
** Voidt is free software: you can redistribute it and/or modify it
** under the terms of the CC BY-NC 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
*******************************************************************/
#ifndef VECTOR3D_H
#define VECTOR3D_H


// TODO(Joey): change floats to lowercase .x and .y due to HLSL/GLSL compatibility.
union vector3D
{
    struct 
    { 
        real32 x;
        real32 y; 
        real32 z; 
    };
    struct 
    { 
        real32 r;
        real32 g; 
        real32 b; 
    };
    // swizzling 
    struct
    {
        vector2D xy;
        real32 _ignored;
    };
    struct
    {
        real32 _ignored;
        vector2D yz;
    };
    real32 E[3];
};


// *
inline vector3D operator*(vector3D a, vector3D b)
{
    return { a.x * b.x, a.y * b.y, a.z * b.z };
}
inline vector3D operator*(vector3D a, real32 b)
{
    return { a.x * b, a.y * b, a.z * b };    
}
inline vector3D operator*(real32 b, vector3D a)
{
    return a * b;
}

// +
inline vector3D operator+(vector3D a, vector3D b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}
// -
inline vector3D operator-(vector3D a, vector3D b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

// -negate
inline vector3D operator-(vector3D a)
{
    return { -a.x, -a.y, -a.z };
}

// *=
inline vector3D& operator*=(vector3D &a, real32 value)
{
    a = a * value;
    return a;
}
inline vector3D& operator*=(vector3D &a, vector3D value)
{
    a = a * value;
    return a;
}
inline vector3D& operator+=(vector3D &a, vector3D value)
{
    a = a + value;
    return a;
}

// ----------------------------------------------------------------------------
//      UTILITY
// ----------------------------------------------------------------------------
inline vector3D Hadamard(vector3D a, vector3D b)
{
    return a * b;
}

inline real32 InnerProduct(vector3D a, vector3D b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline real32 LengthSq(vector3D a)
{
    return InnerProduct(a, a);
}
inline real32 Length(vector3D a)
{
    return SquareRoot(LengthSq(a));
}

inline vector3D Clamp01(vector3D value)
{
    vector3D result;    
    result.x = Clamp01(value.x);
    result.y = Clamp01(value.y);
    result.z = Clamp01(value.z);
    return result;
}

#endif  