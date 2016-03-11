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
        real32 X;
        real32 Y; 
        real32 Z; 
        real32 W; 
    };
    struct 
    { 
        real32 R;
        real32 G; 
        real32 B; 
        real32 A; 
    };
    real32 E[4];
};

// *
inline vector4D operator*(vector4D a, vector4D b)
{
    return { a.X * b.X, a.Y * b.Y, a.Z * b.Z, a.W * b.W };
}
inline vector4D operator*(vector4D a, real32 b)
{
    return { a.X * b, a.Y * b, a.Z * b, a.W * b };    
}
inline vector4D operator*(real32 b, vector4D a)
{
    return a * b;
}

// +
inline vector4D operator+(vector4D a, vector4D b)
{
    return { a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W };
}
// -
inline vector4D operator-(vector4D a, vector4D b)
{
    return { a.X - b.X, a.Y - b.Y, a.Z - b.Z, a.W - b.W };
}

// -negate
inline vector4D operator-(vector4D a)
{
    return { -a.X, -a.Y, -a.Z, -a.W };
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