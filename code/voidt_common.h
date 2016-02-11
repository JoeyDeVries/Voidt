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

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))


#endif