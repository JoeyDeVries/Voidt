#ifndef VOIDT_H
#define VOIDT_H

#include "voidt_platform.h"




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

struct game_state
{
    int32 PlayerTileMapX;
    int32 PlayerTileMapY;
    
    real32 PlayerX;
    real32 PlayerY;
};

struct tile_map
{    
    uint32 *Tiles;
};

struct game_world
{
    int32 CountX;
    int32 CountY;
    
    int32 TileCountX;
    int32 TileCountY;    
    real32 TileWidth;
    real32 TileHeight;
    real32 UpperLeftX;
    real32 UpperLeftY;
    
    tile_map *TileMaps;
};

struct canonical_position
{
    int32 TileMapX;
    int32 TileMapY;

    int32 TileX;
    int32 TileY;
    
    real32 X;
    real32 Y;    
};

struct raw_position
{
    int32 TileMapX;
    int32 TileMapY;
    
    real32 X;
    real32 Y;    
};

#endif