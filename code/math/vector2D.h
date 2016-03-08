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
#ifndef VECTOR2D_H
#define VECTOR2D_H

union vector2D
{
    struct 
    { 
        real32 X;
        real32 Y; 
    };
    real32 E[2];
};

// *
inline vector2D operator*(vector2D a, vector2D b)
{
    return { a.X * b.X, a.Y * b.Y };
}
inline vector2D operator*(vector2D a, real32 b)
{
    return { a.X * b, a.Y * b };    
}
inline vector2D operator*(real32 b, vector2D a)
{
    return a * b;
}

// +
inline vector2D operator+(vector2D a, vector2D b)
{
    return { a.X + b.X, a.Y + b.Y };
}
// NOTE(Joey): scaler vector addition and subtraction isn't really defined, so best to enforce
// proper vector operations by disabling these 'convenience' operators. Instead of adding a 
// scaler, simply add a vector of the proper dimension of that scaler and use that vector to
// add or subtract.
// inline vector2D operator+(vector2D a, real32 b)
// {
    // return { a.X + b, a.Y + b }; 
// }
// inline vector2D operator+(real32 b, vector2D a)
// {
    // return a + b;
// }

// -
inline vector2D operator-(vector2D a, vector2D b)
{
    return { a.X - b.X, a.Y - b.Y };
}

// -negate
inline vector2D operator-(vector2D a)
{
    return { -a.X, -a.Y };
}

// *=
inline vector2D& operator*=(vector2D &a, real32 value)
{
    a = a * value;
    return a;
}
inline vector2D& operator*=(vector2D &a, vector2D value)
{
    a = a * value;
    return a;
}
inline vector2D& operator+=(vector2D &a, vector2D value)
{
    a = a + value;
    return a;
}

// ----------------------------------------------------------------------------
//      UTILITY
// ----------------------------------------------------------------------------
inline vector2D Hadamard(vector2D a, vector2D b)
{
    return a * b;
}

inline real32 InnerProduct(vector2D a, vector2D b)
{
    return a.X * b.X + a.Y * b.Y;
}

inline real32 LengthSq(vector2D a)
{
    return InnerProduct(a, a);
}
inline real32 Length(vector2D a)
{
    return SquareRoot(LengthSq(a));
}

#endif  