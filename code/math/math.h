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
#ifndef MATH_H
#define MATH_H

struct vector2D
{
    union
    {
        struct 
        { 
            real32 X;
            real32 Y; 
        };
        real32 E[2];
    };
    
    inline vector2D& operator*=(vector2D value);
    inline vector2D& operator+=(vector2D value);
    inline vector2D& operator*=(real32 value);
    inline vector2D& operator+=(real32 value);
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
inline vector2D operator+(vector2D a, real32 b)
{
    return { a.X + b, a.Y + b }; 
}
inline vector2D operator+(real32 b, vector2D a)
{
    return a + b;
}

// -
inline vector2D operator-(vector2D a, vector2D b)
{
    return { a.X - b.X, a.Y - b.Y };
}
inline vector2D operator-(vector2D a, real32 b)
{
    return { a.X - b, a.Y - b }; 
}


// -negate
inline vector2D operator-(vector2D a)
{
    return { -a.X, -a.Y };
}

// *=
inline vector2D& vector2D::operator*=(real32 value)
{
    *this = *this * value;
    return *this;
}
inline vector2D& vector2D::operator*=(vector2D value)
{
    *this = *this * value;
    return *this;
}
// +=
inline vector2D& vector2D::operator+=(real32 value)
{
    *this = *this + value;
    return *this;
}
inline vector2D& vector2D::operator+=(vector2D value)
{
    *this = *this + value;
    return *this;
}












#endif