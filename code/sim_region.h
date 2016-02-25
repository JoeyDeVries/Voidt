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

struct sim_entity
{
    uint32 StorageIndex;
    
    entity_type Type;
    
    vector2D Position;  // relative to camera
    // vector2D Velocity;
    uint32 ChunkZ;
    // 
    
    real32 Z;
    real32 dZ;
       
    
    
    vector2D Velocity;
    uint32 FacingDirection;    
    real32 Width;
    real32 Height;
    
    bool32 Collides;
    int32 dAbsTileZ; // for "stairs"
    
    // uint32 HighEntityIndex;
    
    uint32 HitPointMax;
    hit_point HitPoint[16];
    
    entity_reference Sword;
    real32 DistanceRemaining;
    
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
    rectangle2D Bounds;
    
    uint32 MaxEntityCount;
    uint32 EntityCount;
    sim_entity *Entities;    
    
    sim_entity_hash Hash[4096];
};



#endif