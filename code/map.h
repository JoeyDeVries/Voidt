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


const int32 TILE_CHUNK_SAFE_MARGIN = (INT32_MAX/64);
const int32 TILE_CHUNK_UNINITIALIZED = INT32_MAX;

struct tile_map_difference
{
    real32 dX;
    real32 dY;
    real32 dZ;        
};

struct tile_map_position
{
    int32 AbsTileX; // virtual page system (first 28 bits tileMapIndex, last 4 bits tileX)
    int32 AbsTileY;
    int32 AbsTileZ;
    
    vector2D Offset;
};

struct tile_chunk_position
{
    int32 TileChunkX;
    int32 TileChunkY;
    int32 TileChunkZ;
    
    uint32 RelTileX;
    uint32 RelTileY;    
};

struct tile_chunk
{    
    int32 TileChunkX;
    int32 TileChunkY;
    int32 TileChunkZ;
    
    uint32 *Tiles;
    
    tile_chunk *NextInHash;
};

struct tile_map
{
    int32 ChunkShift;
    int32 ChunkMask;
    int32 ChunkDim;
    
    real32 TileSideInMeters;
        
    // number of static tiles per tilemap
    uint32 TileChunkCountX;
    uint32 TileChunkCountY;    
    uint32 TileChunkCountZ;    

    tile_chunk TileChunkHash[4096];
};

#endif