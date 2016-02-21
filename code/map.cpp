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

internal tile_map_position MapIntoTileSpace(tile_map *tileMap, tile_map_position basePos, vector2D offset)
{    
    tile_map_position result = basePos;
    
    result.Offset += offset;
    CorrectTileMapCoord(tileMap, &result.AbsTileX, &result.Offset.X);
    CorrectTileMapCoord(tileMap, &result.AbsTileY, &result.Offset.Y);
    
    return result;
}


inline tile_chunk_position GetChunkPosition(tile_map *tileMap, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    tile_chunk_position result;
    
    result.TileChunkX = absTileX >> tileMap->ChunkShift;
    result.TileChunkY = absTileY >> tileMap->ChunkShift;
    result.TileChunkZ = absTileZ;
    result.RelTileX = absTileX & tileMap->ChunkMask;
    result.RelTileY = absTileY & tileMap->ChunkMask;
    
    return result;
}

const int32 TILE_CHUNK_SAFE_MARGIN = 256;
inline tile_chunk* GetTileChunk(tile_map* tileMap, uint32 tileChunkX, uint32 tileChunkY, uint32 tileChunkZ, memory_arena *arena = 0)
{
    Assert(tileChunkX > TILE_CHUNK_SAFE_MARGIN);
    Assert(tileChunkY > TILE_CHUNK_SAFE_MARGIN);
    Assert(tileChunkZ > TILE_CHUNK_SAFE_MARGIN);
    Assert(tileChunkX < UINT32_MAX - TILE_CHUNK_SAFE_MARGIN);
    Assert(tileChunkY < UINT32_MAX - TILE_CHUNK_SAFE_MARGIN);
    Assert(tileChunkZ < UINT32_MAX - TILE_CHUNK_SAFE_MARGIN);
    
    // TODO(Joey): better hash function
    uint32 hashValue = 19*tileChunkX + 7*tileChunkY + 3*tileChunkZ;
    uint32 hashSlot = hashValue & (ArrayCount(tileMap->TileChunkHash) - 1); // this works because TileChunkHash is power of 2
    Assert(hashSlot < ArrayCount(tileMap->TileChunkHash));
    
    tile_chunk *chunk = tileMap->TileChunkHash;
    do
    {
        if(chunk->TileChunkX != tileChunkX &&       
           chunk->TileChunkY != tileChunkY &&
           chunk->TileChunkZ != tileChunkZ)
       {
           break;
       }
       
       if(arena && chunk->TileChunkX != 0 && !chunk->NextInHash)
       {
            chunk->NextInHash = PushStruct(arena, tile_chunk);
            chunk->TileChunkX = 0;
            chunk = chunk->NextInHash;
       }
        
        if(arena && !chunk->TileChunkX == 0)
        {
            chunk->TileChunkX = tileChunkX;
            chunk->TileChunkY = tileChunkY;
            chunk->TileChunkZ = tileChunkZ;
    
            uint32 tileCount = tileMap->ChunkDim*tileMap->ChunkDim;            
            chunk->Tiles = PushArray(arena, tileCount, uint32);
            
            for(uint32 tileIndex = 0; tileIndex < tileCount; ++tileIndex)
                chunk->Tiles[tileIndex] = 1;    

            chunk->NextInHash = 0;
            
            break;
        }
    } while(chunk);
    return chunk;
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

inline uint32 GetTileValue(tile_map *tileMap, tile_map_position pos)
{  
    return GetTileValue(tileMap, pos.AbsTileX, pos.AbsTileY, pos.AbsTileZ);
}

inline bool32 IsTileValueEmpty(uint32 tileValue)
{
    return tileValue == 1 || tileValue == 3 || tileValue == 4;
}

internal bool32 IsTileMapPointEmpty(tile_map *tileMap, tile_map_position pos)
{
    uint32 tileChunkValue = GetTileValue(tileMap, pos.AbsTileX, pos.AbsTileY, pos.AbsTileZ);
    bool32 empty = IsTileValueEmpty(tileChunkValue);
    return empty;
}

internal void SetTileValue(memory_arena *arena, tile_map *tileMap, uint32 absTileX, uint32 absTileY, uint32 absTileZ, uint32 tileValue)
{
    tile_chunk_position chunkPos = GetChunkPosition(tileMap, absTileX, absTileY, absTileZ);
    tile_chunk *tileChunk = GetTileChunk(tileMap, chunkPos.TileChunkX, chunkPos.TileChunkY, chunkPos.TileChunkZ, arena);
    
    Assert(tileChunk);

    SetTileValue(tileMap, tileChunk, chunkPos.RelTileX, chunkPos.RelTileY, tileValue);
}

inline bool32
AreOnSameTile(tile_map_position *A, tile_map_position *B)
{
    bool32 Result = ((A->AbsTileX == B->AbsTileX) &&
                     (A->AbsTileY == B->AbsTileY) &&
                     (A->AbsTileZ == B->AbsTileZ));

    return(Result);
}

tile_map_difference Subtract(tile_map *tileMap, tile_map_position *a, tile_map_position *b)
{
    tile_map_difference result;
    
    real32 dTileX = (real32)a->AbsTileX - (real32)b->AbsTileX;
    real32 dTileY = (real32)a->AbsTileY - (real32)b->AbsTileY;
    real32 dTileZ = (real32)a->AbsTileZ - (real32)b->AbsTileZ;
    
    result.dX = tileMap->TileSideInMeters*dTileX + (a->Offset.X - b->Offset.X);
    result.dY = tileMap->TileSideInMeters*dTileY + (a->Offset.Y - b->Offset.Y);
    result.dZ = tileMap->TileSideInMeters*dTileZ;
    
    return result;
}

inline tile_map_position
CenteredTilePoint(uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    tile_map_position Result = {};

    Result.AbsTileX = absTileX;
    Result.AbsTileY = absTileY;
    Result.AbsTileZ = absTileZ;

    return(Result);
}


internal void InitializeTileMap(tile_map *tileMap, real32 tileSideInMeters)
{
    tileMap->ChunkShift = 4;
    tileMap->ChunkMask = (1 << tileMap->ChunkShift) - 1;
    tileMap->ChunkDim = (1 << tileMap->ChunkShift);
    tileMap->TileSideInMeters = tileSideInMeters;
    
    for(uint32 tileChunkIndex = 0; tileChunkIndex < ArrayCount(tileMap->TileChunkHash); ++tileChunkIndex)
    {
        tileMap->TileChunkHash[tileChunkIndex].TileChunkX = 0;
    }
}