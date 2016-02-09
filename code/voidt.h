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


struct tile_chunk
{    
    uint32 *Tiles;
};

struct game_world
{
    uint32 ChunkShift;
    uint32 ChunkMask;
    uint32 ChunkDim;
    
    real32 TileSideInMeters;
    int32  TileSideInPixels;
    real32 MetersToPixels;
    
    // dimensions of tilemaps array
    int32 CountX;
    int32 CountY;
    
    // number of static tiles per tilemap
    int32 TileChunkCountX;
    int32 TileChunkCountY;    

    tile_chunk *TileChunks;
};

struct tile_chunk_position
{
    uint32 TileChunkX;
    uint32 TileChunkY;
    
    uint32 RelTileX;
    uint32 RelTileY;    
};

struct world_position
{
    uint32 AbsTileX; // virtual page system (first 28 bits tileMapIndex, last 4 bits tileX)
    uint32 AbsTileY;
    
    real32 OffsetX;
    real32 OffsetY;    
};

struct game_state
{
    world_position PlayerPos;
};


#endif