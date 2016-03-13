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
#include "voidt_platform.h"
#include "intrinsics.h"
#include "math/math.h"
#include "map.h"
#include "sim_region.h"
#include "entity.h"

#include "common/random.h"
#include "common/random.cpp"
#include "renderer/renderer.h"
#include "renderer/renderer.cpp"


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


inline void InitializeArena(memory_arena *arena, memory_index size, void *base)
{
    arena->Size = size;
    arena->Base = (uint8*)base;
    arena->Used = 0;
}

#define PushStruct(arena, type) (type*)PushSize_(arena, sizeof(type))
#define PushArray(arena, count, type) (type*)PushSize_(arena, (count)*sizeof(type))
inline void* PushSize_(memory_arena *arena, memory_index size)
{
    Assert(arena->Used + size <= arena->Size);
    void* address = arena->Base + arena->Used;
    arena->Used += size;
    
    // clear to zero (redundant for now)    
    // ZeroSize(size, address);
    // for(uint8* memory = (uint8*)address; memory < arena->Base + arena->Used + size; ++memory)
        // *memory = 0;
    
    return address;
}
#define ZeroStruct(instance) ZeroSize(sizeof(instance), &instance)
inline void ZeroSize(memory_index size, void *ptr)
{
    // TODO(Joey): check for performance
    uint8 *byte = (uint8*)ptr;
    while(size--)
    {
        *byte++ = 0;
    }
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
    vector2D Align;
    loaded_bitmap Head;
    loaded_bitmap Torso;
    loaded_bitmap Cape;
    
};

struct high_entity
{
  
};





struct low_entity
{
    sim_entity Sim;
    world_position Position;
};

// struct game_entity
// {      
    // uint32 LowIndex;
    // high_entity *High;
    // low_entity *Low;
// };


struct entity_visible_piece
{
    loaded_bitmap *Bitmap;
    vector2D Offset;
    real32 OffsetZ;
    real32 EntityZC;
    
    real32 R, G, B, A;
    vector2D Dimension;
};

struct controlled_player
{
    uint32 EntityIndex;
    vector3D Velocity;
    vector2D Acceleration;
    vector2D AccelerationSword;
    // real32 dZ;
};

struct pairwise_collision_rule
{
    bool32 ShouldCollide;
    uint32 StorageIndexA;
    uint32 StorageIndexB;
    
    pairwise_collision_rule *NextInHash;
};

struct game_state
{
    memory_arena WorldArena;
    game_world *World;
    
    uint32 CameraFollowingEntityIndex;
    world_position CameraPos;
    
    controlled_player ControlledPlayers[ArrayCount(((game_input*)0)->Controllers)];
    
    uint32 HighEntityCount;
    high_entity HighEntities[256];

    uint32 LowEntityCount;
    low_entity LowEntities[100000];
    
    real32 MetersToPixels;
    
    loaded_bitmap BackDrop;
    loaded_bitmap Shadow;
    loaded_bitmap Sword;
    loaded_bitmap Stairwell;
    hero_bitmaps HeroBitmaps[4];
    
    loaded_bitmap Tree;
    
    // NOTE(Joey): must be power of 2 (for now)
    pairwise_collision_rule *CollisionRuleHash[256];
    pairwise_collision_rule *FirstFreeCollisionRule;
};

struct entity_visible_piece_group
{
    game_state *GameState;
    
    uint32 PieceCount;
    entity_visible_piece Pieces[32];
};

inline low_entity* GetLowEntity(game_state *gameState, uint32 lowIndex)
{
    low_entity *result = 0;
    if(lowIndex > 0 && lowIndex < gameState->LowEntityCount)
    {
        result = gameState->LowEntities + lowIndex;
    }
    return result;
}

internal void AddCollisionRule(game_state *gameState, uint32 storageIndexA, uint32 storageIndexB, bool32 shouldCollide);
internal void ClearCollisionRules(game_state *gameState, uint32 storageIndex);

#endif