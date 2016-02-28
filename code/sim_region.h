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
#ifndef SIMULATION_REGION_H
#define SIMULATION_REGION_H

struct move_spec
{
    bool32 UnitMaxAccelVector;
    real32 Speed;
    real32 Drag;
};

enum entity_type
{
    ENTITY_TYPE_NULL,
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_WALL,    
    ENTITY_TYPE_FAMILIAR,
    ENTITY_TYPE_MONSTER,
    ENTITY_TYPE_SWORD,
};

#define HIT_POINT_SUB_COUNT 4
struct hit_point
{
    uint8 Flags;
    uint8 FilledAmount;
};

struct sim_entity;
union entity_reference
{
    sim_entity *Ptr;
    uint32 Index;
};

enum sim_entity_flags :uint32_t
{
    ENTITY_FLAG_COLLIDES = (1 << 0),
    ENTITY_FLAG_NONSPATIAL = (1 << 1),
    
    
    ENTITY_FLAG_SIMMING = (1 << 30),    
};


struct sim_entity
{
    uint32 StorageIndex;
    bool32 Updatable;
    
    entity_type Type;
    uint32 Flags;
    
    vector3D Position;  // relative to camera
    vector3D Velocity;
    // uint32 ChunkZ;
    // 
    
    // real32 Z;
    // real32 dZ;
    
    real32 DistanceLimit;
       
    
    
    uint32 FacingDirection;    
    real32 Width;
    real32 Height;
    
    // bool32 Collides;
    int32 dAbsTileZ; // for "stairs"
    
    // uint32 HighEntityIndex;
    
    uint32 HitPointMax;
    hit_point HitPoint[16];
    
    entity_reference Sword;
};

struct sim_entity_hash
{
    sim_entity *Ptr;
    uint32 Index;
};

struct sim_region
{
    game_world *World;
    
    world_position Origin;
    rectangle3D Bounds;
    rectangle3D UpdateBounds;
    
    uint32 MaxEntityCount;
    uint32 EntityCount;
    sim_entity *Entities;    
    
    sim_entity_hash Hash[4096];
};



#endif