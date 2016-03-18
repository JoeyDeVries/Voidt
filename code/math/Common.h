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
#ifndef COMMON_H
#define COMMON_H

// ----------------------------------------------------------------------------
//      ARITHMETIC
// ----------------------------------------------------------------------------
internal real32 Square(real32 a)
{
    return a * a;
}

// ----------------------------------------------------------------------------
//      ALGEBRA
// ----------------------------------------------------------------------------
inline real32 Clamp(real32 min, real32 max, real32 value)
{
    real32 result = value;    
    if(result < min) result = min;
    if(result > max) result = max;
    return result;
}

inline real32 Clamp01(real32 value)
{
    real32 result = Clamp(0.0f, 1.0f, value);
    return result;
}

inline real32 Lerp(real32 a, real32 b, real32 t)
{
    return (1.0f - t)*a + t*b; 
}


#endif