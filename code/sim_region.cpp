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
internal sim_entity_hash* GetHashFromStorageIndex(sim_region *simRegion, uint32 storageIndex)
{
    Assert(storageIndex);
    sim_entity_hash *result = 0;

    uint32 hashValue = storageIndex;        
    for(uint32 offset = 0; offset < ArrayCount(simRegion->Hash); ++offset)
    {
        // NOTE(Joey): mask high bits of array size to mask out overflow indices
        sim_entity_hash *entry = simRegion->Hash + ((hashValue + offset) & (ArrayCount(simRegion->Hash) - 1));
        if(entry->Index == 0 || entry->Index == storageIndex)
        {
            result = entry;
            break;
        }
    }    
    return result;
}

// internal void MapStorageIndexToEntity(sim_region *simRegion, uint32 storageIndex, sim_entity *entity)
// {
    // sim_entity_hash *entry = GetHashFromStorageIndex(simRegion, storageIndex);
    // Assert(entry->Index == 0 || entry->Index == storageIndex);
    // entry->Index = storageIndex;
    // entry->Ptr = entity;
// }

inline sim_entity* GetEntityByStorageIndex(sim_region *simRegion, uint32 storageIndex)
{
    sim_entity_hash *entry = GetHashFromStorageIndex(simRegion, storageIndex);
    sim_entity *result = entry->Ptr;
    return result;
}

internal sim_entity* AddEntity(game_state *gameState, sim_region *simRegion, uint32 storageIndex, low_entity *source, vector2D *simPos);
inline void LoadEntityReference(game_state *gameState, sim_region *simRegion, entity_reference *ref)
{
    if(ref->Index)
    {
        sim_entity_hash *entry = GetHashFromStorageIndex(simRegion, ref->Index);
        
        if(entry->Ptr == 0)
        {
            entry->Ptr = AddEntity(gameState, simRegion, ref->Index, GetLowEntity(gameState, ref->Index), 0);
        }
        
        ref->Ptr = entry->Ptr;
    }
}

inline void StoreEntityReference(entity_reference *ref)
{
    if(ref->Ptr != 0)
        ref->Index = ref->Ptr->StorageIndex;
}


internal sim_entity* AddEntityRaw(game_state *gameState, sim_region *simRegion, uint32 storageIndex, low_entity *source)
{
    Assert(storageIndex);
    sim_entity *entity = 0;
    
    sim_entity_hash *entry = GetHashFromStorageIndex(simRegion, storageIndex);        
    if(entry->Ptr == 0)
    {
        if(simRegion->EntityCount < simRegion->MaxEntityCount)
        {
            entity = &simRegion->Entities[simRegion->EntityCount++];
            
            entry->Index = storageIndex;
            entry->Ptr = entity;
                    
            if(source)
            {
                *entity = source->Sim; // copy stored state back into sim_entity
                LoadEntityReference(gameState, simRegion, &entity->Sword);
                
                Assert(!IsSet(&source->Sim, ENTITY_FLAG_SIMMING));
                SetFlag(&source->Sim, ENTITY_FLAG_SIMMING);
            }
            entity->StorageIndex = storageIndex;
        }
        else
            InvalidCodePath;
    }
    return entity;
}


inline vector2D GetSimSpacePos(sim_region *simRegion, low_entity *stored)
{
    // TODO(Joey): set this to signal NAN in debug mode, to make sure noone ever 
    // uses this position.
    vector2D result = INVALID_POS;
    if(!IsSet(&stored->Sim, ENTITY_FLAG_NONSPATIAL))
    {
        world_difference diff = Subtract(simRegion->World, &stored->Position, &simRegion->Origin);
        result = { diff.dX, diff.dY };    
    }
    return result;
}


internal sim_entity* AddEntity(game_state *gameState, sim_region *simRegion, uint32 storageIndex, low_entity *source, vector2D *simPos)
{
    sim_entity *dest = AddEntityRaw(gameState, simRegion, storageIndex, source);
    if(dest)
    {
        if(simPos)
        {
            dest->Position = *simPos;
        }
        else
        {
            dest->Position = GetSimSpacePos(simRegion, source);
        }
    }
    return dest;
}

internal sim_region* BeginSimulation(memory_arena *simArena, game_state *gameState, game_world *world, world_position origin, rectangle2D bounds)
{       
    sim_region *simRegion = PushStruct(simArena, sim_region);
    ZeroStruct(simRegion->Hash);
    
    simRegion->World = world;
    simRegion->Bounds = bounds;
    simRegion->Origin = origin;
    
    simRegion->MaxEntityCount = 4096;
    simRegion->EntityCount = 0;
    simRegion->Entities = PushArray(simArena, simRegion->MaxEntityCount, sim_entity);
    
    
    
    world_position minChunkPos = MapIntoChunkSpace(world, simRegion->Origin, GetMinCorner(simRegion->Bounds));
    world_position maxChunkPos = MapIntoChunkSpace(world, simRegion->Origin, GetMaxCorner(simRegion->Bounds));
    
    for(int32 chunkY = minChunkPos.ChunkY; chunkY <= maxChunkPos.ChunkY; ++chunkY)
    {
        for(int32 chunkX = minChunkPos.ChunkX; chunkX <= maxChunkPos.ChunkX; ++chunkX)
        {
            world_chunk *chunk = GetWorldChunk(world, chunkX, chunkY, simRegion->Origin.ChunkZ);
            if(chunk)
            {
                for(world_entity_block *block = &chunk->FirstBlock; block; block = block->Next)
                {
                    for(uint32 entityIndexIndex = 0; entityIndexIndex < block->EntityCount; ++entityIndexIndex)
                    {
                        uint32 lowEntityIndex = block->LowEntityIndex[entityIndexIndex];
                        low_entity *low = gameState->LowEntities + lowEntityIndex;         
                        if(!IsSet(&low->Sim, ENTITY_FLAG_NONSPATIAL))
                        {
                            vector2D simSpacePos = GetSimSpacePos(simRegion, low);
                            if(IsInRectangle(simRegion->Bounds, simSpacePos))
                            {
                                AddEntity(gameState, simRegion, lowEntityIndex, low, &simSpacePos);
                            }
                        }
                    }
                }
            }
        }
    }        
    return simRegion;
}

internal void EndSimulation(sim_region *region, game_state *gameState)
{
    sim_entity *entity = region->Entities;
    for(uint32 entityIndex = 0; entityIndex < region->EntityCount; ++entityIndex, ++entity)
    {
        low_entity *stored = gameState->LowEntities + entity->StorageIndex;
     
        Assert(IsSet(&stored->Sim, ENTITY_FLAG_SIMMING));
        stored->Sim = *entity; // store current state of entity
        Assert(!IsSet(&stored->Sim, ENTITY_FLAG_SIMMING));
        
        StoreEntityReference(&stored->Sim.Sword);
        
        
        world_position newPos = IsSet(entity, ENTITY_FLAG_NONSPATIAL) ? 
            NullPosition() :
            MapIntoChunkSpace(gameState->World, region->Origin, entity->Position);
        ChangeEntityLocation(&gameState->WorldArena, gameState->World, entity->StorageIndex, stored, newPos);
        
        if(entity->StorageIndex == gameState->CameraFollowingEntityIndex)
        {
            world_position newCameraP = gameState->CameraPos;  
            
            newCameraP.ChunkZ = stored->Position.ChunkZ;
        
            // if(entity->Position.X > 9.0f*world->TileSideInMeters)
                // newCameraP.ChunkX += 17;
            // if(entity->Position.X < -9.0f*world->TileSideInMeters)
                // newCameraP.ChunkX -= 17;
            // if(entity->Position.Y > 5.0f*world->TileSideInMeters)
                // newCameraP.ChunkY += 9;
            // if(entity->Position.Y < -5.0f*world->TileSideInMeters)
                // newCameraP.ChunkY -= 9;
            
            gameState->CameraPos = newCameraP;       
        }   
  
    }
    
    // after simulation, update camera
    // sim_entity *CameraFollowingEntity = GetEntityByStorageIndex(region, gameState->CameraFollowingEntityIndex);
    // if(cameraFollowingEntity)
    // if(entity->StorageIndex == gameState->CameraFollowingEntityIndex)
    // {
        // world_position newCameraP = gameState->CameraPos;
        
        // newCameraP.ChunkZ = cameraFollowingEntity.Low->Position.ChunkZ;
    
        // if(cameraFollowingEntity.High->Position.X > 9.0f*world->TileSideInMeters)
            // newCameraP.ChunkX += 17;
        // if(cameraFollowingEntity.High->Position.X < -9.0f*world->TileSideInMeters)
            // newCameraP.ChunkX -= 17;
        // if(cameraFollowingEntity.High->Position.Y > 5.0f*world->TileSideInMeters)
            // newCameraP.ChunkY += 9;
        // if(cameraFollowingEntity.High->Position.Y < -5.0f*world->TileSideInMeters)
            // newCameraP.ChunkY -= 9;
        
        // newCameraP = cameraFollowingEntity.Low->Position;
        
        // SetCamera(gameState, newCameraP);
    // }   
}


internal bool32 TestWall(real32 wallX, real32 relX, real32 relY, real32 playerDeltaX, real32 playerDeltaY, real32 *tMin, real32 minY, real32 maxY)
{
    real32 tEpsilon = 0.01f;
    if(playerDeltaX != 0.0f)
    {
        real32 tResult = (wallX - relX) / playerDeltaX;
        real32 y = relY + tResult*playerDeltaY;
        if(tResult >= 0.0f && *tMin > tResult)
        {
            if(y >= minY && y <= maxY)
            {
                *tMin = Maximum(0.0f, tResult - tEpsilon);
                return true;
            }
        }        
    }    
    return false;
}    


internal void MoveEntity(sim_region *simRegion, sim_entity *entity, real32 dt, move_spec *moveSpec, vector2D acceleration)
{
    Assert(!IsSet(entity, ENTITY_FLAG_NONSPATIAL));
    
    game_world *world = simRegion->World;
    
    if(moveSpec->UnitMaxAccelVector)
    {
        real32 accelerationLength = LengthSq(acceleration); // get squared length, still valid to check length > 1.0 and much cheaper (no sqrt)
        if(accelerationLength > 1.0f)
        {
            acceleration *= 1.0f / SquareRoot(accelerationLength);
        }
    }    
    acceleration *= moveSpec->Speed;
            
    // add friction (approximation) to acceleration
    acceleration += -moveSpec->Drag*entity->Velocity;
            
    // update entity position (use velocity at begin of frame)
    vector2D oldPlayerPos = entity->Position;
    vector2D playerDelta = 0.5f * acceleration*Square(dt) + entity->Velocity*dt;
    vector2D newPlayerPos = oldPlayerPos + playerDelta;
    entity->Velocity += acceleration * dt;
            
         
    // detect collisions         
    // real32 tRemaining = 1.0f;      
    for(uint32 i = 0; i < 4; ++i)
    {
        real32 tMin = 1.0f;
        vector2D wallNormal = {};                
        sim_entity *hitEntity = 0;
        
        vector2D desiredPosition = entity->Position + playerDelta;
        
        if(IsSet(entity, ENTITY_FLAG_COLLIDES) && !IsSet(entity, ENTITY_FLAG_NONSPATIAL))
        {
            for(uint32 entityIndex = 0; entityIndex < simRegion->EntityCount; ++entityIndex)
            {
                /* NOTE(Joey):
                   
                  We're going to use the Minkowski sum for collision detection.
                  The idea is to reduce the relative complex shapes of objects to check
                  by adding the shape of one along the exterior of the other to reduce
                  one of the shapes to a single point. Collision detection then simply
                  becomes a point-shape algorithm, which is relatively easier to implement.
                 
                  At the moment we simply only deal with quads (for more complex shapes we
                  want to use the GJK algorithm). We solve this by adding entity-quad's 
                  halfwidths to the shape(s) to test, reducing the player entity to a 
                  point.
                */               
                sim_entity *testEntity = simRegion->Entities + entityIndex;
                if(entity != testEntity)
                {                 
                    if(IsSet(testEntity, ENTITY_FLAG_COLLIDES) && !IsSet(entity, ENTITY_FLAG_NONSPATIAL))
                    {
                        real32 diameterW = testEntity->Width  + entity->Width;
                        real32 diameterH = testEntity->Height + entity->Height;
                        vector2D minCorner = -0.5f*vector2D{diameterW, diameterH};
                        vector2D maxCorner = 0.5f*vector2D{diameterW, diameterH};

                        vector2D rel = entity->Position - testEntity->Position;
                 
                        if(TestWall(minCorner.X, rel.X, rel.Y, playerDelta.X, playerDelta.Y,
                                    &tMin, minCorner.Y, maxCorner.Y))
                        {
                            wallNormal = {-1, 0};
                            hitEntity = testEntity;
                        }
                    
                        if(TestWall(maxCorner.X, rel.X, rel.Y, playerDelta.X, playerDelta.Y,
                                    &tMin, minCorner.Y, maxCorner.Y))
                        {
                            wallNormal = {1, 0};
                            hitEntity = testEntity;
                        }
                    
                        if(TestWall(minCorner.Y, rel.Y, rel.X, playerDelta.Y, playerDelta.X,
                                    &tMin, minCorner.X, maxCorner.X))
                        {
                            wallNormal = {0, -1};
                            hitEntity = testEntity;
                        }
                    
                        if(TestWall(maxCorner.Y, rel.Y, rel.X, playerDelta.Y, playerDelta.X,
                                    &tMin, minCorner.X, maxCorner.X))
                        {
                            wallNormal = {0, 1};
                            hitEntity = testEntity;
                        }     
                    }
                }            
            }    
            entity->Position += tMin*playerDelta;    
            // special collision events, should something happen? (like stairs, cinematic start areas)
            if(hitEntity)
            {
                entity->Velocity = entity->Velocity - 1*InnerProduct(entity->Velocity, wallNormal)*wallNormal;
                playerDelta = desiredPosition - entity->Position;
                playerDelta = playerDelta - 1*InnerProduct(playerDelta, wallNormal)*wallNormal;
                // tRemaining -= tMin*tRemaining;
                
                // high_entity *hitHigh = gameState->HighEntities + hitHighEntityIndex;
                // low_entity  *hitLow  = gameState->LowEntities  + hitHigh->LowEntityIndex;
                // entity->AbsTileZ += hitLow->dAbsTileZ;
            }
            else
                break;
        }
    }                         
    
    // determine new facing direction
    if(entity->Velocity.X == 0.0f && entity->Velocity.Y == 0.0f)
    {
        // NOTE(Joey): leave facing direction to earlier set facing direction
    }
    else if(Absolute(entity->Velocity.X) > Absolute(entity->Velocity.Y))
    {
        entity->FacingDirection = entity->Velocity.X > 0 ? 0 : 2;            
    }
    else
    {
        entity->FacingDirection = entity->Velocity.Y > 0 ? 1 : 3;
    }       
}

// internal void SimCameraRegion(game_state *gameState)
// {
    // determine how far camera moved, and update all entities back to proper camera space
    // game_world *world = gameState->World;
    
    // uint32 tileSpanX = 17*3;
    // uint32 tileSpanY =  9*3;
    // rectangle2D cameraBounds = RectCenterDim(vector2D { 0, 0 }, world->TileSideInMeters*vector2D { (real32)tileSpanX, (real32)tileSpanY });                 
      
    // sim_region *simRegion = BeginSim(simArena, world, gameState->CameraPos, cameraBounds);
    
    // EndSim(simRegion, gameState);
// }