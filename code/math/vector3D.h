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

union vector3D
{
    struct 
    { 
        real32 X;
        real32 Y; 
        real32 Z; 
    };
    struct 
    { 
        real32 R;
        real32 G; 
        real32 B; 
    };
    // swizzling 
    struct
    {
        vector2D XY;
        real32 _ignored;
    };
    struct
    {
        real32 _ignored;
        vector2D YZ;
    };
    real32 E[3];
};


// *
inline vector3D operator*(vector3D a, vector3D b)
{
    return { a.X * b.X, a.Y * b.Y, a.Z * b.Z };
}
inline vector3D operator*(vector3D a, real32 b)
{
    return { a.X * b, a.Y * b, a.Z * b };    
}
inline vector3D operator*(real32 b, vector3D a)
{
    return a * b;
}

// +
inline vector3D operator+(vector3D a, vector3D b)
{
    return { a.X + b.X, a.Y + b.Y, a.Z + b.Z };
}
// -
inline vector3D operator-(vector3D a, vector3D b)
{
    return { a.X - b.X, a.Y - b.Y, a.Z - b.Z };
}

// -negate
inline vector3D operator-(vector3D a)
{
    return { -a.X, -a.Y, -a.Z };
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
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
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
    result.X = Clamp01(value.X);
    result.Y = Clamp01(value.Y);
    result.Z = Clamp01(value.Z);
    return result;
}

#endif  