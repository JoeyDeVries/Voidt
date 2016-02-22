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
#ifndef VOIDT_H
#define VOIDT_H

#include "common/voidt_common.h"
#include "math/math.h"
#include "voidt_platform.h"
#include "map.h"


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


internal void InitializeArena(memory_arena *arena, memory_index size, uint8 *base)
{
    arena->Size = size;
    arena->Base = base;
    arena->Used = 0;
}

#define PushStruct(arena, type) (type*)PushSize_(arena, sizeof(type))
#define PushArray(arena, count, type) (type*)PushSize_(arena, (count)*sizeof(type))
internal void* PushSize_(memory_arena *arena, memory_index size)
{
    Assert(arena->Used + size <= arena->Size);
    void* address = arena->Base + arena->Used;
    arena->Used += size;
    
    // clear to zero (redundant for now)    
    for(uint8* memory = (uint8*)address; memory < arena->Base + arena->Used + size; ++memory)
        *memory = 0;
    
    return address;
}


// struct game_world
// {
    // tile_map *TileMap;   
// };

struct loaded_bitmap
{
    int32 Width;
    int32 Height;
    uint32 *Pixels;
};

struct hero_bitmaps
{
    int32 AlignX;
    int32 AlignY;
    loaded_bitmap Head;
    loaded_bitmap Torso;
    loaded_bitmap Cape;
    
};

struct high_entity
{
    vector2D Position;  // relative to camera
    vector2D Velocity;
    uint32 AbsTileZ;
    uint32 FacingDirection;    
    
    real32 Z;
    real32 dZ;
    
    uint32 LowEntityIndex;
};

enum entity_type
{
    ENTITY_TYPE_NULL,
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_WALL,    
};

struct low_entity
{
    entity_type Type;
    
    world_position Position;
    real32 Width;
    real32 Height;
    
    bool32 Collides;
    int32 dAbsTileZ; // for "stairs"
    
    uint32 HighEntityIndex;
};

struct game_entity
{      
    uint32 LowIndex;
    high_entity *High;
    low_entity *Low;
};

// struct low_entity_chunk_reference
// {
    // tile_chunk *TileChunk;
    // uint32 EntityIndexInChunk;
// };

struct game_state
{
    memory_arena WorldArena;
    game_world *World;
    
    uint32 CameraFollowingEntityIndex;
    world_position CameraPos;
    
    uint32 PlayerControllerIndex[ArrayCount(((game_input*)0)->Controllers)];
    
    uint32 HighEntityCount;
    high_entity HighEntities[256];

    uint32 LowEntityCount;
    low_entity LowEntities[1000000];
    
    
    loaded_bitmap BackDrop;
    loaded_bitmap Shadow;
    hero_bitmaps HeroBitmaps[4];
    
};


#endif