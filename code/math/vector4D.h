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
#ifndef VECTOR4D_H
#define VECTOR4D_H


// TODO(Joey): change floats to lowercase .x and .y due to HLSL/GLSL compatibility.
union vector4D
{
    struct 
    { 
        real32 x;
        real32 y; 
        real32 z; 
        real32 w; 
    };
    struct 
    { 
        real32 r;
        real32 g; 
        real32 b; 
        real32 a; 
    };
    real32 E[4];
};

// *
inline vector4D operator*(vector4D a, vector4D b)
{
    return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}
inline vector4D operator*(vector4D a, real32 b)
{
    return { a.x * b, a.y * b, a.z * b, a.w * b };    
}
inline vector4D operator*(real32 b, vector4D a)
{
    return a * b;
}

// +
inline vector4D operator+(vector4D a, vector4D b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}
// -
inline vector4D operator-(vector4D a, vector4D b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

// -negate
inline vector4D operator-(vector4D a)
{
    return { -a.x, -a.y, -a.z, -a.w };
}

// *=
inline vector4D& operator*=(vector4D &a, real32 value)
{
    a = a * value;
    return a;
}
inline vector4D& operator*=(vector4D &a, vector4D value)
{
    a = a * value;
    return a;
}
inline vector4D& operator+=(vector4D &a, vector4D value)
{
    a = a + value;
    return a;
}

// ----------------------------------------------------------------------------
//      UTILITY
// ----------------------------------------------------------------------------
inline vector4D Hadamard(vector4D a, vector4D b)
{
    return a * b;
}

#endif  