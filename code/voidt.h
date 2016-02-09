#ifndef VOIDT_H
#define VOIDT_H

#include "voidt_platform.h"
#include "map.h"

// ----------------------------------------------------------------------------
//      PRE-PROCESSOR UTILITY
// ----------------------------------------------------------------------------
#define internal        static
#define local_persist   static
#define global_variable static

#if DEBUG
#define Assert(Expression) if(!(Expression)) { *(int *) 0 = 0; }
#else
#define Assert(Expression)
#endif

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))



inline game_controller_input* GetController(game_input *input, uint32 controllerIndex)
{
    Assert(controllerIndex < ArrayCount(input->Controllers));
    game_controller_input *result = &input->Controllers[controllerIndex];
    return result;
}

struct memory_arena 
{
    uint8 *Base;
    memory_index Size;
    memory_index Used;
};

struct game_world
{
    tile_map *TileMap;   
};

struct game_state
{
    memory_arena WorldArena;
    game_world *World;
    tile_map_position PlayerPos;
};


#endif