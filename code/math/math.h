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


// ----------------------------------------------------------------------------
//      ARITHMETIC
// ----------------------------------------------------------------------------
internal real32 Square(real32 a)
{
    return a * a;
}


// ----------------------------------------------------------------------------
//      VECTOR 2D
// ----------------------------------------------------------------------------
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
//      VECTOR 3D
// ----------------------------------------------------------------------------
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
//      VECTOR 4D
// ----------------------------------------------------------------------------
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



// globals
// static const vector2D 2D_UP = vector2D{0.0f, 1.0f};



// NOTE(Joey): we already do Hadamard product im multiplication operator, but these are for readability
inline vector2D Hadamard(vector2D a, vector2D b)
{
    return a * b;
}
inline vector3D Hadamard(vector3D a, vector3D b)
{
    return a * b;
}
inline vector4D Hadamard(vector4D a, vector4D b)
{
    return a * b;
}

inline real32 InnerProduct(vector2D a, vector2D b)
{
    return a.X * b.X + a.Y * b.Y;
}
inline real32 InnerProduct(vector3D a, vector3D b)
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

inline real32 LengthSq(vector2D a)
{
    return InnerProduct(a, a);
}
inline real32 Length(vector2D a)
{
    return SquareRoot(LengthSq(a));
}
inline real32 LengthSq(vector3D a)
{
    return InnerProduct(a, a);
}
inline real32 Length(vector3D a)
{
    return SquareRoot(LengthSq(a));
}



// ----------------------------------------------------------------------------
//      RECTANGLE 2D
// ----------------------------------------------------------------------------
struct rectangle2D
{
    vector2D Min;
    vector2D Max;    
};

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
    bool32 result = test.X >= rectangle.Min.X &&
                    test.Y >= rectangle.Min.Y &&
                    test.X  < rectangle.Max.X &&
                    test.Y  < rectangle.Max.Y;
    
    return true;
}

// ----------------------------------------------------------------------------
//      RECTANGLE 3D
// ----------------------------------------------------------------------------
struct rectangle3D
{
    vector3D Min;
    vector3D Max;    
};

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
    bool32 result = !((b.Max.X < a.Min.X || b.Min.X > a.Max.X) ||
                      (b.Max.Y < a.Min.Y || b.Min.Y > a.Max.Y) ||
                      (b.Max.Z < a.Min.Z || b.Min.Z > a.Max.Z));
    return result;
}

inline vector3D GetBaryCentric(rectangle3D rect, vector3D pos)
{
    return (pos - rect.Min) / length((rect.Max - rect.Minx));
}

// ----------------------------------------------------------------------------
//      COMMON
// ----------------------------------------------------------------------------

inline real32 Clamp(real32 min, real32 max, real32 value)
{
    real32 result = value;    
    if(result < min) result = min;
    if(result > max) result = max;
}

inline real32 Clamp01(real32 value)
{
    real32 result = Clamp(0.0f, 1.0f, value);
    return result;
}
inline vector3D Clamp01(vector3D value)
{
    vector3D result;    
    result.X = Clamp01(value.X);
    result.Y = Clamp01(value.Y);
    result.Z = Clamp01(value.Z);
    return result;
}

inline real32 lerp(real32 a, real32 b, real32 t)
{
    return (1.0f - t)*a + t*b; 
}

#endif