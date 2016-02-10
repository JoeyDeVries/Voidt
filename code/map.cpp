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

internal void CorrectTileMapCoord(tile_map *tileMap, uint32 *tile, real32 *tileRel)
{
    int32 tileOverflow = RoundReal32ToInt32(*tileRel / tileMap->TileSideInMeters);
    *tile    += tileOverflow;
    *tileRel -= tileOverflow*tileMap->TileSideInMeters;
    
    Assert(*tileRel >= -0.5f*tileMap->TileSideInMeters);
    Assert(*tileRel <=  0.5f*tileMap->TileSideInMeters);   
}

internal tile_map_position CorrectTileMapPosition(tile_map *tileMap, tile_map_position pos)
{    
    tile_map_position result = pos;
    
    CorrectTileMapCoord(tileMap, &result.AbsTileX, &result.OffsetX);
    CorrectTileMapCoord(tileMap, &result.AbsTileY, &result.OffsetY);
    
    return result;
}


inline tile_chunk_position GetChunkPosition(tile_map *tileMap, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    tile_chunk_position result;
    
    result.TileChunkX = absTileX >> tileMap->ChunkShift;
    result.TileChunkY = absTileY >> tileMap->ChunkShift;
    result.TileChunkZ = absTileZ >> tileMap->ChunkShift;
    result.RelTileX = absTileX & tileMap->ChunkMask;
    result.RelTileY = absTileY & tileMap->ChunkMask;
    
    return result;
}

inline tile_chunk* GetTileChunk(tile_map* tileMap, uint32 tileChunkX, uint32 tileChunkY, uint32 tileChunkZ)
{
    tile_chunk *tileChunk = 0;
    if(tileChunkX >= 0 && tileChunkX < tileMap->TileChunkCountX && 
       tileChunkY >= 0 && tileChunkY < tileMap->TileChunkCountY && 
       tileChunkZ >= 0 && tileChunkZ < tileMap->TileChunkCountZ)
    {
        tileChunk = &tileMap->TileChunks[tileChunkZ*tileMap->TileChunkCountY*tileMap->TileChunkCountX + 
                                         tileChunkY*tileMap->TileChunkCountX + 
                                         tileChunkX];
    }
    return tileChunk;
}

inline uint32 GetTileValueUnchecked(tile_map *tileMap, tile_chunk *chunk, uint32 tileX, uint32 tileY)
{
    Assert(chunk);
    Assert(tileX < tileMap->ChunkDim);
    Assert(tileY < tileMap->ChunkDim);
    
    return chunk->Tiles[tileY*tileMap->ChunkDim + tileX];
}

inline void SetTileValueUnchecked(tile_map *tileMap, tile_chunk *chunk, uint32 tileX, uint32 tileY, uint32 tileValue)
{
    Assert(chunk);
    Assert(tileX < tileMap->ChunkDim);
    Assert(tileY < tileMap->ChunkDim);
    
    chunk->Tiles[tileY*tileMap->ChunkDim + tileX] = tileValue;
}


inline uint32 GetTileValue(tile_map *tileMap, tile_chunk *tileChunk, uint32 tileX, uint32 tileY)
{
    uint32 tileValue = 0;
    if(tileChunk && tileChunk->Tiles)
        tileValue = GetTileValueUnchecked(tileMap, tileChunk, tileX, tileY);
    return tileValue;
}

inline void SetTileValue(tile_map *tileMap, tile_chunk *tileChunk, uint32 tileX, uint32 tileY, uint32 tileValue)
{
    if(tileChunk && tileChunk->Tiles)
        SetTileValueUnchecked(tileMap, tileChunk, tileX, tileY, tileValue);
}

inline uint32 GetTileValue(tile_map *tileMap, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{  
    tile_chunk_position chunkPos = GetChunkPosition(tileMap, absTileX, absTileY, absTileZ);
    tile_chunk *tileChunk = GetTileChunk(tileMap, chunkPos.TileChunkX, chunkPos.TileChunkY, chunkPos.TileChunkZ);
    uint32 tileChunkValue = GetTileValue(tileMap, tileChunk, chunkPos.RelTileX, chunkPos.RelTileY);
    
    return tileChunkValue;
}

internal bool32 IsTileMapPointEmpty(tile_map *tileMap, tile_map_position pos)
{
    uint32 tileChunkValue = GetTileValue(tileMap, pos.AbsTileX, pos.AbsTileY, pos.AbsTileZ);
    bool32 empty = tileChunkValue == 1;
    return empty;
}

internal void SetTileValue(memory_arena *arena, tile_map *tileMap, uint32 absTileX, uint32 absTileY, uint32 absTileZ, uint32 tileValue)
{
    tile_chunk_position chunkPos = GetChunkPosition(tileMap, absTileX, absTileY, absTileZ);
    tile_chunk *tileChunk = GetTileChunk(tileMap, chunkPos.TileChunkX, chunkPos.TileChunkY, chunkPos.TileChunkZ);
    
    Assert(tileChunk);
    
    if(!tileChunk->Tiles)
    {
        uint32 tileCount = tileMap->ChunkDim*tileMap->ChunkDim;
        tileChunk->Tiles = PushArray(arena, tileCount, uint32);
        
        // initialize with default values
        for(uint32 tileIndex = 0; tileIndex < tileCount; ++tileIndex)
        {
            tileChunk->Tiles[tileIndex] = 1;
        }
        
    }
    
    SetTileValue(tileMap, tileChunk, chunkPos.RelTileX, chunkPos.RelTileY, tileValue);
}