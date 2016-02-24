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

inline world_position NullPosition()
{
    world_position result = {};
    result.ChunkX = WORLD_CHUNK_UNINITIALIZED;
    return result;
}

inline bool32 IsValid(world_position pos)
{
    return pos.ChunkX != WORLD_CHUNK_UNINITIALIZED;
}

inline bool32 IsCanonical(game_world *world, real32 rel)
{
    bool32 result = rel >= -0.5f*world->ChunkSideInMeters && rel <=  0.5f*world->ChunkSideInMeters;
    return result;
}

inline bool32 IsCanonical(game_world *world, vector2D offset)
{
    bool32 result = IsCanonical(world, offset.X) && IsCanonical(world, offset.Y);
    return result;
}


internal void CorrectChunkCoord(game_world *world, int32 *tile, real32 *tileRel)
{
    int32 tileOverflow = RoundReal32ToInt32(*tileRel / world->ChunkSideInMeters);
    *tile    += tileOverflow;
    *tileRel -= tileOverflow*world->ChunkSideInMeters;
    
    Assert(IsCanonical(world, *tileRel));
   
}

internal world_position MapIntoChunkSpace(game_world *world, world_position basePos, vector2D offset)
{    
    world_position result = basePos;
    
    result.Offset += offset;
    CorrectChunkCoord(world, &result.ChunkX, &result.Offset.X);
    CorrectChunkCoord(world, &result.ChunkY, &result.Offset.Y);
    
    return result;
}

inline bool32
AreInSameChunk(game_world *world, world_position *A, world_position *B)
{
    Assert(IsCanonical(world, A->Offset));
    Assert(IsCanonical(world, B->Offset));
    bool32 Result = ((A->ChunkX == B->ChunkX) &&
                     (A->ChunkY == B->ChunkY) &&
                     (A->ChunkZ == B->ChunkZ));

    return(Result);
}


inline world_chunk* GetWorldChunk(game_world* world, int32 chunkX, int32 chunkY, int32 chunkZ, memory_arena *arena = 0)
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
        
        chunk = chunk->NextInHash;
    } while(chunk);
    return chunk;
}

inline world_position ChunkPositionFromTilePosition(game_world *world, int32 absTileX, int32 absTileY, int32 absTileZ)
{
    world_position result = {};
    
    result.ChunkX = absTileX / TILES_PER_CHUNK;
    result.ChunkY = absTileY / TILES_PER_CHUNK;
    result.ChunkZ = absTileZ;
    
    result.Offset.X = (real32)((absTileX - TILES_PER_CHUNK/2) - (result.ChunkX * (real32)TILES_PER_CHUNK)) * world->TileSideInMeters;
    result.Offset.Y = (real32)((absTileY - TILES_PER_CHUNK/2)- (result.ChunkY * (real32)TILES_PER_CHUNK)) * world->TileSideInMeters;
    
    Assert(IsCanonical(world, result.Offset));
    
    return result;
}

world_difference Subtract(game_world *world, world_position *a, world_position *b)
{
    world_difference result;
    
    real32 dChunkX = (real32)a->ChunkX - (real32)b->ChunkX;
    real32 dChunkY = (real32)a->ChunkY - (real32)b->ChunkY;
    real32 dChunkZ = (real32)a->ChunkZ - (real32)b->ChunkZ;
    
    result.dX = world->ChunkSideInMeters*dChunkX + (a->Offset.X - b->Offset.X);
    result.dY = world->ChunkSideInMeters*dChunkY + (a->Offset.Y - b->Offset.Y);
    result.dZ = world->ChunkSideInMeters*dChunkZ;
    
    return result;
}

inline world_position
CenteredChunkPoint(uint32 chunkX, uint32 chunkY, uint32 chunkZ)
{
    world_position Result = {};

    Result.ChunkX = chunkX;
    Result.ChunkY = chunkY;
    Result.ChunkZ = chunkZ;

    return(Result);
}

internal void InitializeWorld(game_world *world, real32 tileSideInMeters)
{
    world->TileSideInMeters = tileSideInMeters;
    world->ChunkSideInMeters = (real32)TILES_PER_CHUNK*tileSideInMeters;
    world->FirstFree = 0;
    
    for(uint32 chunkIndex = 0; chunkIndex < ArrayCount(world->ChunkHash); ++chunkIndex)
    {
        world->ChunkHash[chunkIndex].ChunkX = WORLD_CHUNK_UNINITIALIZED;
        world->ChunkHash[chunkIndex].FirstBlock.EntityCount = 0;
    }
}

inline void ChangeEntityLocationRaw(memory_arena *arena, game_world *world, uint32 lowEntityIndex, world_position *oldPos, world_position *newPos)
{
    Assert(!oldPos || IsValid(*oldPos));
    Assert(!newPos || IsValid(*newPos));
    
    if(oldPos && newPos && AreInSameChunk(world, oldPos, newPos))
    {
        // do nothing, leave entity where it is
    }
    else
    {
        if(oldPos)
        {
            // pull the entity out of its current block
            world_chunk *chunk = GetWorldChunk(world, oldPos->ChunkX, oldPos->ChunkY, oldPos->ChunkZ);
            Assert(chunk);
            
            if(chunk)
            {
                bool32 notFound = true;
                world_entity_block *firstBlock = &chunk->FirstBlock;
                for(world_entity_block *block = firstBlock; block && notFound; block = block->Next)
                {
                    for(uint32 index = 0; index < block->EntityCount && notFound; ++index)
                    {
                        if(block->LowEntityIndex[index] == lowEntityIndex)
                        {
                            // NOTE(Joey): we keep the empty spots at the first block instead of the last blocks
                            // so we can do quick inserts; this means whenever we remove a low entity from a block
                            // we swap the now empty place with one from the first block to free space at the head.
                            Assert(firstBlock->EntityCount > 0);
                            block->LowEntityIndex[index] = firstBlock->LowEntityIndex[--firstBlock->EntityCount];
                            if(firstBlock->EntityCount == 0) // first block is empty, swap next full block as first block
                            {
                                if(firstBlock->Next)
                                {
                                    world_entity_block *nextBlock = firstBlock->Next;
                                    *firstBlock = *nextBlock;
                                    
                                    nextBlock->Next = world->FirstFree;
                                    world->FirstFree = nextBlock;
                                }                        
                            }
                            
                            notFound = false;
                        }
                    }
                }
            }
        }
        
        if(newPos)
        {
            world_chunk* chunk = GetWorldChunk(world, newPos->ChunkX, newPos->ChunkY, newPos->ChunkZ, arena);
            Assert(chunk);
            
            world_entity_block *block = &chunk->FirstBlock;
            if(block->EntityCount == ArrayCount(block->LowEntityIndex))
            {
                // NOTE(Joey): we're out of room, get a new block
                world_entity_block *oldBlock = world->FirstFree;
                if(oldBlock)
                {
                    world->FirstFree = oldBlock->Next; 
                }
                else
                {
                    oldBlock = PushStruct(arena, world_entity_block);
                }
                *oldBlock = *block;
                block->Next = oldBlock;
                block->EntityCount = 0;            
            }
            
            Assert(block->EntityCount < ArrayCount(block->LowEntityIndex));
            block->LowEntityIndex[block->EntityCount++] = lowEntityIndex;
        }
    }
}

internal void ChangeEntityLocation(memory_arena *arena, game_world *world, uint32 lowEntityIndex, low_entity *lowEntity, world_position *oldPos, world_position *newPos)
{
    ChangeEntityLocationRaw(arena, world, lowEntityIndex, oldPos, newPos);
    if(newPos)
        lowEntity->Position = *newPos;
    else
        lowEntity->Position = NullPosition();
}  