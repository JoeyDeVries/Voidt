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
#ifndef RECTANGLE3D_H
#define RECTANGLE3D_H

struct rectangle3D
{
    vector3D Min;
    vector3D Max;    
};

// ----------------------------------------------------------------------------
//      UTILITY
// ----------------------------------------------------------------------------
inline vector3D GetMinCorner(rectangle3D rectangle)
{
    return rectangle.Min;
}

inline vector3D GetMaxCorner(rectangle3D rectangle)
{
    return rectangle.Max;
}

inline vector3D GetCenter(rectangle3D rectangle)
{
    return 0.5f*(rectangle.Min + rectangle.Max);
}

inline rectangle3D RectMinMax(vector3D min, vector3D max)
{
    return { min, max };
}

inline rectangle3D RectMinDim(vector3D min, vector3D dim)
{
    return { min, min + dim };
}

inline rectangle3D RectCenterHalfDim(vector3D center, vector3D halfDim)
{
    rectangle3D result;
    
    result.Min = center - halfDim;
    result.Max = center + halfDim;
    
    return result;
}

inline rectangle3D RectCenterDim(vector3D center, vector3D dim)
{
    return RectCenterHalfDim(center, 0.5f*dim);
}

inline rectangle3D AddRadius(rectangle3D rectangle, vector3D radius)
{
    rectangle3D result;
    
    result.Min = rectangle.Min - radius;
    result.Max = rectangle.Max + radius;
    
    return result;
}

inline bool32 IsInRectangle(rectangle3D rectangle, vector3D test)
{
    bool32 result = test.X >= rectangle.Min.X &&
                    test.Y >= rectangle.Min.Y &&
                    test.X  < rectangle.Max.X &&
                    test.Y  < rectangle.Max.Y &&
                    test.Z >= rectangle.Min.Z &&
                    test.Z  < rectangle.Max.Z;
    
    return true;
}

inline bool32 RectanglesIntersect(rectangle3D a, rectangle3D b)
{
    // NOTE(Joey): AABB intersection check on basis of individual axis
    bool32 result = !((b.Max.X <= a.Min.X || b.Min.X >= a.Max.X) ||
                      (b.Max.Y <= a.Min.Y || b.Min.Y >= a.Max.Y) ||
                      (b.Max.Z <= a.Min.Z || b.Min.Z >= a.Max.Z));
    return result;
}

inline vector3D GetBaryCentric(rectangle3D rect, vector3D pos)
{
    real32 x = (pos.X - rect.Min.X) / Length(rect.Max - rect.Min);
    real32 y = (pos.Y - rect.Min.Y) / Length(rect.Max - rect.Min);
    real32 z = (pos.Z - rect.Min.Z) / Length(rect.Max - rect.Min);
    return { x, y, z };
}

#endif  