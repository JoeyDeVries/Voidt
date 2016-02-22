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

internal void CorrectTileMapCoord(game_world *world, int32 *tile, real32 *tileRel)
{
    int32 tileOverflow = RoundReal32ToInt32(*tileRel / world->TileSideInMeters);
    *tile    += tileOverflow;
    *tileRel -= tileOverflow*world->TileSideInMeters;
    
    Assert(*tileRel >= -0.5f*world->TileSideInMeters);
    Assert(*tileRel <=  0.5f*world->TileSideInMeters);   
}

internal world_position MapIntoTileSpace(game_world *world, world_position basePos, vector2D offset)
{    
    world_position result = basePos;
    
    result.Offset += offset;
    CorrectTileMapCoord(world, &result.AbsTileX, &result.Offset.X);
    CorrectTileMapCoord(world, &result.AbsTileY, &result.Offset.Y);
    
    return result;
}


// inline world_chunk_position GetChunkPosition(game_world *world, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
// {
    // world_chunk_position result;
    
    // result.TileChunkX = absTileX >> world->ChunkShift;
    // result.TileChunkY = absTileY >> world->ChunkShift;
    // result.TileChunkZ = absTileZ;
    // result.RelTileX = absTileX & world->ChunkMask;
    // result.RelTileY = absTileY & world->ChunkMask;
    
    // return result;
// }

inline world_chunk* GetWorldChunkChunk(game_world* world, int32 chunkX, int32 chunkY, int32 chunkZ, memory_arena *arena = 0)
{
    Assert(chunkX > -WORLD_CHUNK_SAFE_MARGIN);
    Assert(chunkY > -WORLD_CHUNK_SAFE_MARGIN);
    Assert(chunkZ > -WORLD_CHUNK_SAFE_MARGIN);
    Assert(chunkX <  WORLD_CHUNK_SAFE_MARGIN);
    Assert(chunkY <  WORLD_CHUNK_SAFE_MARGIN);
    Assert(chunkZ <  WORLD_CHUNK_SAFE_MARGIN);
    
    // TODO(Joey): better hash function
    uint32 hashValue = 19*chunkX + 7*chunkY + 3*chunkZ;
    uint32 hashSlot = hashValue & (ArrayCount(world->ChunkHash) - 1); // this works because ChunkHash is power of 2
    Assert(hashSlot < ArrayCount(world->ChunkHash));
    
    world_chunk *chunk = world->ChunkHash + hashSlot;
    do
    {
        if(chunk->ChunkX == chunkX &&       
           chunk->ChunkY == chunkY &&
           chunk->ChunkZ == chunkZ)
       {
           break;
       }
       
       if(arena && (chunk->ChunkX != WORLD_CHUNK_UNINITIALIZED) && (!chunk->NextInHash))
       {
            chunk->NextInHash = PushStruct(arena, world_chunk);
            chunk = chunk->NextInHash;
            chunk->ChunkX = WORLD_CHUNK_UNINITIALIZED;
       }
        
        if(arena && (chunk->ChunkX == WORLD_CHUNK_UNINITIALIZED))
        {
            chunk->ChunkX = chunkX;
            chunk->ChunkY = chunkY;
            chunk->ChunkZ = chunkZ;

            
            chunk->NextInHash = 0;
            
            break;
        }
    } while(chunk);
    return chunk;
}

// inline uint32 GetTileValueUnchecked(tile_map *tileMap, world_chunk *chunk, int32 tileX, int32 tileY)
// {
    // Assert(chunk);
    // Assert(tileX < tileMap->ChunkDim);
    // Assert(tileY < tileMap->ChunkDim);
    
    // return chunk->Tiles[tileY*tileMap->ChunkDim + tileX];
// }

// inline void SetTileValueUnchecked(tile_map *tileMap, world_chunk *chunk, int32 tileX, int32 tileY, uint32 tileValue)
// {
    // Assert(chunk);
    // Assert(tileX < tileMap->ChunkDim);
    // Assert(tileY < tileMap->ChunkDim);
    
    // chunk->Tiles[tileY*tileMap->ChunkDim + tileX] = tileValue;
// }


// inline uint32 GetTileValue(tile_map *tileMap, world_chunk *chunk, uint32 tileX, uint32 tileY)
// {
    // uint32 tileValue = 0;
    // if(chunk && chunk->Tiles)
        // tileValue = GetTileValueUnchecked(tileMap, chunk, tileX, tileY);
    // return tileValue;
// }

// inline void SetTileValue(tile_map *tileMap, world_chunk *chunk, uint32 tileX, uint32 tileY, uint32 tileValue)
// {
    // if(chunk && chunk->Tiles)
        // SetTileValueUnchecked(tileMap, chunk, tileX, tileY, tileValue);
// }

// inline uint32 GetTileValue(tile_map *tileMap, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
// {  
    // world_chunk_position chunkPos = GetChunkPosition(tileMap, absTileX, absTileY, absTileZ);
    // world_chunk *chunk = GetTileChunk(tileMap, chunkPos.TileChunkX, chunkPos.TileChunkY, chunkPos.TileChunkZ);
    // uint32 chunkValue = GetTileValue(tileMap, chunk, chunkPos.RelTileX, chunkPos.RelTileY);
    
    // return chunkValue;
// }

// inline uint32 GetTileValue(tile_map *tileMap, tile_map_position pos)
// {  
    // return GetTileValue(tileMap, pos.AbsTileX, pos.AbsTileY, pos.AbsTileZ);
// }

// inline bool32 IsTileValueEmpty(uint32 tileValue)
// {
    // return tileValue == 1 || tileValue == 3 || tileValue == 4;
// }

// internal bool32 IsTileMapPointEmpty(tile_map *tileMap, tile_map_position pos)
// {
    // uint32 chunkValue = GetTileValue(tileMap, pos.AbsTileX, pos.AbsTileY, pos.AbsTileZ);
    // bool32 empty = IsTileValueEmpty(chunkValue);
    // return empty;
// }

// internal void SetTileValue(memory_arena *arena, tile_map *tileMap, uint32 absTileX, uint32 absTileY, uint32 absTileZ, uint32 tileValue)
// {
    // world_chunk_position chunkPos = GetChunkPosition(tileMap, absTileX, absTileY, absTileZ);
    // world_chunk *chunk = GetTileChunk(tileMap, chunkPos.TileChunkX, chunkPos.TileChunkY, chunkPos.TileChunkZ, arena);
    
    // Assert(chunk);

    // SetTileValue(tileMap, chunk, chunkPos.RelTileX, chunkPos.RelTileY, tileValue);
// }

inline bool32
AreOnSameTile(world_position *A, world_position *B)
{
    bool32 Result = ((A->AbsTileX == B->AbsTileX) &&
                     (A->AbsTileY == B->AbsTileY) &&
                     (A->AbsTileZ == B->AbsTileZ));

    return(Result);
}

world_difference Subtract(game_world *world, world_position *a, world_position *b)
{
    world_difference result;
    
    real32 dTileX = (real32)a->AbsTileX - (real32)b->AbsTileX;
    real32 dTileY = (real32)a->AbsTileY - (real32)b->AbsTileY;
    real32 dTileZ = (real32)a->AbsTileZ - (real32)b->AbsTileZ;
    
    result.dX = world->TileSideInMeters*dTileX + (a->Offset.X - b->Offset.X);
    result.dY = world->TileSideInMeters*dTileY + (a->Offset.Y - b->Offset.Y);
    result.dZ = world->TileSideInMeters*dTileZ;
    
    return result;
}

inline world_position
CenteredTilePoint(uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    world_position Result = {};

    Result.AbsTileX = absTileX;
    Result.AbsTileY = absTileY;
    Result.AbsTileZ = absTileZ;

    return(Result);
}


internal void InitializeWorld(game_world *world, real32 tileSideInMeters)
{
    world->ChunkShift = 4;
    world->ChunkMask = (1 << world->ChunkShift) - 1;
    world->ChunkDim  = (1 << world->ChunkShift);
    world->TileSideInMeters = tileSideInMeters;
    
    for(uint32 chunkIndex = 0; chunkIndex < ArrayCount(world->ChunkHash); ++chunkIndex)
    {
        world->ChunkHash[chunkIndex].ChunkX = WORLD_CHUNK_UNINITIALIZED;
    }
}

inline void ChangeEntityLocation(memory_arena, *arena, game_world *world, uint32 lowEntityIndex, world_position *oldPos, world_position *newPos)
{
    if(OldPos && AreInSameChunk(oldPos, newPos))
    {
        // do nothing, leave entity where it is
    }
    else
    {
        if(OldPos)
        {
            // pull the entity out of its current block
            world_chunk *chunk = GetWorldChunk(world, oldPos->ChunkX, oldPos->ChunkY, oldPos->ChunkZ);
            Assert(chunk);
            
            for(world_entity_block *block = &chunk->FirstBlock; block; block = block->Next)
            {
                
            }
        }
        
        world_chunk* chunk = GetWorldChunk(world, newPos->ChunkX, newPos->ChunkY, nwePos->ChunkZ, arena);
        world_entity_block *block = &chunk->FirstBlock;
        if(chunk->EntityCount == ArrayCount(chunk->LowEntityIndex))
        {
            // NOTE(Joey): we're out of room, get a new block
            world_entity_block *oldBlock = PushStruct(arena, world_entity_block);
            *oldBlock = *block;
            *block->Next = oldBlock;
            block->EntityCount = 0;            
        }
        
        Assert(block->EntityCount < ArrayCount(block->LowEntityIndex));
        block->LowEntityIndex[block->EntityCount++] = lowEntityIndex;
    }
}