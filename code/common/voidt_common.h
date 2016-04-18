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
#ifndef VOIDT_COMMON
#define VOIDT_COMMON

// ----------------------------------------------------------------------------
//      PLATFORM-INDEPENDENT TYPES
// ----------------------------------------------------------------------------
#include <stdint.h> // fixed-size types 
#include <stddef.h> // size_t

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32   bool32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef size_t memory_index;

typedef float  real32;
typedef double real64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef int32   b32;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  r32;
typedef double r64;

// ----------------------------------------------------------------------------
//      PRE-PROCESSOR DEFINITIONS
// ----------------------------------------------------------------------------
#define internal        static
#define local_persist   static
#define global_variable static

// constants
#define Pi32 3.141592665359f

// ----------------------------------------------------------------------------
//      PRE-PROCESSOR UTILITY
// ----------------------------------------------------------------------------
#if DEBUG
#define Assert(Expression) if(!(Expression)) { *(int *) 0 = 0; }
#else
#define Assert(Expression)
#endif

#define InvalidCodePath Assert(!"InvalidCodePath");

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

// ----------------------------------------------------------------------------
//      PRE-PROCESSOR UTILITY
// ----------------------------------------------------------------------------

#endif