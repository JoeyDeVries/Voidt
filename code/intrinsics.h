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
#ifndef VOIDT_INTRINSICS_H
#define VOIDT_INTRINSICS_H
#include "math.h"

inline int32 SignOf(int32 value)
{
    int32 result = value >= 0 ? 1 : -1;
    return result;
}

inline real32 SquareRoot(real32 value)
{
    return sqrtf(value);
}

inline real32 Absolute(real32 value)
{
    return (real32)fabs(value);
}

inline real32 Min(real32 a, real32 b)
{
    return a <= b ? a : b;
}

inline real32 Max(real32 a, real32 b)
{
    return a >= b ? a : b;
}

inline real32 SafeRatio(r32 a, r32 b)
{
    if(b == 0.0f)
        return 0.0f;
    
    return a / b;
}

inline real64 SafeRatio(r64 a, r64 b)
{
    if(b == 0.0f)
        return 0.0f;
    
    return a / b;
}

inline uint32 RotateLeft(uint32 value, uint32 amount)
{
#if COMPILER_MSVC
    return _rotl(value, amount);
#else
    // TODO(Joey): port to other compiler platform
    amount &= 31;
    return (value << amount) | (value >> (32 - amount);
#endif
}

inline uint32 RotateRight(uint32 value, uint32 amount)
{
#if COMPILER_MSVC
    return _rotr(value, amount);
#else
    // TODO(Joey): port to other compiler platform
    amount &= 31;
    return (value >> amount) | (value << (32 - amount)
#endif
}


inline int32 RoundReal32ToInt32(real32 value)
{
    return (int32)roundf(value);
}

inline uint32 RoundReal32ToUInt32(real32 value)
{
    return (uint32)roundf(value);
}


inline int32 FloorReal32ToInt32(real32 value)
{
    return (int32)floorf(value);
}

inline int32 CeilReal32ToInt32(real32 value)
{
    return (int32)ceilf(value);
}


inline real32 Sin(real32 angle)
{
    return sinf(angle);
}

inline real32 Cos(real32 angle)
{
    return cosf(angle);
}

inline real32 ATan2(real32 y, real32 x)
{
    return atan2f(y, x);
}

internal bool32 FindLeastSignificantSetBit(uint32 *index, uint32 value)
{
    
#if COMPILER_MSVC
    return _BitScanForward((unsigned long*)index, value);
#else
    for(int i = 0; i < 32; ++i)
    {
        if(value & (1 << i))
        {
            *index = i;
            return true;
        }
    }
    return false;
#endif
}

#endif