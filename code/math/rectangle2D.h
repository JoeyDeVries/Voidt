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
#ifndef RECTANGLE2D_H
#define RECTANGLE2D_H

struct rectangle2D
{
    vector2D Min;
    vector2D Max;    
};

// ----------------------------------------------------------------------------
//      UTILITY
// ----------------------------------------------------------------------------
inline vector2D GetMinCorner(rectangle2D rectangle)
{
    return rectangle.Min;
}

inline vector2D GetMaxCorner(rectangle2D rectangle)
{
    return rectangle.Max;
}

inline vector2D GetCenter(rectangle2D rectangle)
{
    return 0.5f*(rectangle.Min + rectangle.Max);
}

inline rectangle2D RectMinMax(vector2D min, vector2D max)
{
    return { min, max };
}

inline rectangle2D RectMinDim(vector2D min, vector2D dim)
{
    return { min, min + dim };
}

inline rectangle2D RectCenterHalfDim(vector2D center, vector2D halfDim)
{
    rectangle2D result;
    
    result.Min = center - halfDim;
    result.Max = center + halfDim;
    
    return result;
}

inline rectangle2D RectCenterDim(vector2D center, vector2D dim)
{
    return RectCenterHalfDim(center, 0.5f*dim);
}

inline rectangle2D AddRadius(rectangle2D rectangle, vector2D radius)
{
    rectangle2D result;
    
    result.Min = rectangle.Min - radius;
    result.Max = rectangle.Max + radius;
    
    return result;
}

inline bool32 IsInRectangle(rectangle2D rectangle, vector2D test)
{
    bool32 result = test.x >= rectangle.Min.x &&
                    test.y >= rectangle.Min.y &&
                    test.x  < rectangle.Max.x &&
                    test.y  < rectangle.Max.y;
    
    return true;
}

#endif  