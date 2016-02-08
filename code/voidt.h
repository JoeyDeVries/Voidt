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


struct tile_map
{    
    uint32 *Tiles;
};

struct game_world
{
    real32 TileSideInMeters;
    int32  TileSideInPixels;
    real32 MetersToPixels;
    
    // dimensions of tilemaps array
    int32 CountX;
    int32 CountY;
    
    // number of static tiles per tilemap
    int32 TileCountX;
    int32 TileCountY;    

    real32 LowerLeftX;
    real32 LowerLeftY;
    
    tile_map *TileMaps;
};

struct world_position
{
    int32 TileMapX;
    int32 TileMapY;

    int32 TileX; // virtual page system (first 28 bits tileMapIndex, last 4 bits tileX)
    int32 TileY;
    
    real32 TileRelX;
    real32 TileRelY;    
};

struct game_state
{
    world_position PlayerPos;
};


#endif