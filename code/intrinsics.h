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

inline int32 RoundReal32ToInt32(real32 value)
{
    return (int32)roundf(value);
}

inline uint32 RoundReal32ToUint32(real32 value)
{
    return (uint32)roundf(value);
}


inline int32 FloorReal32ToInt32(real32 value)
{
    return (uint32)floorf(value);
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


#endif