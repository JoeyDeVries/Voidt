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
#ifndef VOIDT_MAP_H
#define VOIDT_MAP_H


const int32 WORLD_CHUNK_SAFE_MARGIN = (INT32_MAX/64);
#define WORLD_CHUNK_UNINITIALIZED INT32_MAX

struct world_difference
{
    real32 dX;
    real32 dY;
    real32 dZ;        
};

struct world_position
{
    int32 ChunkX; // virtual page system (first 28 bits tileMapIndex, last 4 bits tileX)
    int32 ChunkY;
    int32 ChunkZ;
    
    vector2D Offset;
};

struct world_entity_block
{
    uint32 EntityCount;
    uint32 LowEntityIndex[16];
    world_entity_block *Next;
};

struct world_chunk
{    
    int32 ChunkX, ChunkY, ChunkZ;
    
    world_entity_block FirstBlock;
    
    world_chunk *NextInHash;
};

struct game_world
{
    real32 TileSideInMeters;
    real32 ChunkSideInMeters;

    world_chunk ChunkHash[4096];
};

#endif