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

struct tile_map_difference
{
    real32 dX;
    real32 dY;
    real32 dZ;        
};

struct tile_map_position
{
    uint32 AbsTileX; // virtual page system (first 28 bits tileMapIndex, last 4 bits tileX)
    uint32 AbsTileY;
    uint32 AbsTileZ;
    
    vector2D Offset;
};

struct tile_chunk_position
{
    uint32 TileChunkX;
    uint32 TileChunkY;
    uint32 TileChunkZ;
    
    uint32 RelTileX;
    uint32 RelTileY;    
};

struct tile_chunk
{    
    uint32 *Tiles;
};

struct tile_map
{
    uint32 ChunkShift;
    uint32 ChunkMask;
    uint32 ChunkDim;
    
    real32 TileSideInMeters;
        
    // number of static tiles per tilemap
    uint32 TileChunkCountX;
    uint32 TileChunkCountY;    
    uint32 TileChunkCountZ;    

    tile_chunk *TileChunks;
};

#endif