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

inline sim_entity* GetEntityByStorageIndex(sim_region *simRegion, uint32 storageIndex)
{
    sim_entity_hash *entry = GetHashFromStorageIndex(simRegion, storageIndex);
    sim_entity *result = entry->Ptr;
    return result;
}

inline vector3D GetSimSpacePos(sim_region *simRegion, low_entity *stored)
{
    // TODO(Joey): set this to signal NAN in debug mode, to make sure noone ever 
    // uses this position.
    vector3D result = INVALID_POS;
    if(!IsSet(&stored->Sim, ENTITY_FLAG_NONSPATIAL))
    {
        result = Subtract(simRegion->World, &stored->Position, &simRegion->Origin);
        // result = diff.XY;    
    }
    return result;
}


internal sim_entity* AddEntity(game_state *gameState, sim_region *simRegion, uint32 storageIndex, low_entity *source, vector3D *simPos);
inline void LoadEntityReference(game_state *gameState, sim_region *simRegion, entity_reference *ref)
{
    if(ref->Index)
    {
        sim_entity_hash *entry = GetHashFromStorageIndex(simRegion, ref->Index);
        
        if(entry->Ptr == 0)
        {
            entry->Index = ref->Index;
            low_entity *lowEntity = GetLowEntity(gameState, ref->Index);
            vector3D pos = GetSimSpacePos(simRegion, lowEntity);
            entry->Ptr = AddEntity(gameState, simRegion, ref->Index, lowEntity, &pos);
        }
        
        ref->Ptr = entry->Ptr;
    }
}

inline void StoreEntityReference(entity_reference *ref)
{
    if(ref->Ptr != 0)
        ref->Index = ref->Ptr->StorageIndex;
}

inline bool32 EntityOverlapsRectangle(rectangle3D rect, vector3D pos, vector3D size)
{
    rectangle3D grown = AddRadius(rect, 0.5f*size);
    return IsInRectangle(grown, pos);
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
            entity->Updatable = false;                      
        }
        else
            InvalidCodePath;
    }
    return entity;
}


internal sim_entity* AddEntity(game_state *gameState, sim_region *simRegion, uint32 storageIndex, low_entity *source, vector3D *simPos)
{
    sim_entity *dest = AddEntityRaw(gameState, simRegion, storageIndex, source);
    if(dest)
    {
        if(simPos)
        {
            dest->Position = *simPos;
            dest->Updatable = EntityOverlapsRectangle(simRegion->UpdateBounds, dest->Position, dest->Size);    
        }
        else
        {
            dest->Position = GetSimSpacePos(simRegion, source);
        }
    }
    return dest;
}

internal sim_region* BeginSimulation(memory_arena *simArena, game_state *gameState, game_world *world, real32 dt, world_position origin, rectangle3D bounds)
{       
    sim_region *simRegion = PushStruct(simArena, sim_region);
    ZeroStruct(simRegion->Hash);
    
    // TODO(Joey): calculate eventually from maximum value an entity may move + radius
    simRegion->MaxEntityRadius = 5.0f;
    simRegion->MaxEntityVelocity = 30.0f; 
    real32 updateSafetyMargin = simRegion->MaxEntityRadius + dt*simRegion->MaxEntityVelocity;
    real32 updateSafetyMarginZ = 1.0f;
    
    simRegion->World = world;
    simRegion->UpdateBounds = AddRadius(bounds, vector3D { simRegion->MaxEntityRadius, simRegion->MaxEntityRadius, simRegion->MaxEntityRadius });
    simRegion->Bounds = AddRadius(simRegion->UpdateBounds, { updateSafetyMargin, updateSafetyMargin, updateSafetyMarginZ });
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
                            vector3D simSpacePos = GetSimSpacePos(simRegion, low);
                            if(EntityOverlapsRectangle(simRegion->Bounds, simSpacePos, low->Sim.Size))
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
            
            real32 prevCamZ = newCameraP.Offset.Z;
            newCameraP = stored->Position;
            newCameraP.Offset.Z = prevCamZ;
            gameState->CameraPos = newCameraP;       
        }   
  
    }    
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

inline bool32 TypesMatch(sim_entity *a, sim_entity *b, entity_type aType, entity_type bType)
{
    return a->Type == aType && b->Type == bType;
}



internal bool32 ShouldCollide(game_state *gameState, sim_entity *a, sim_entity *b)
{
    bool32 result = false;
    
    if(a != b)
    {
        if(a->StorageIndex > b->StorageIndex) 
        {
            sim_entity *temp = a;
            a = b;
            b = temp;
        }
        
        if(!IsSet(a, ENTITY_FLAG_NONSPATIAL) && !IsSet(b, ENTITY_FLAG_NONSPATIAL))
        {
            result = true;        
        }
        
        uint32 hashValue = a->StorageIndex & (ArrayCount(gameState->CollisionRuleHash) - 1);    
        for(pairwise_collision_rule *rule = gameState->CollisionRuleHash[hashValue];
            rule;
            rule = rule->NextInHash)
        {
            if(rule->StorageIndexA == a->StorageIndex && rule->StorageIndexB == b->StorageIndex)
            {
                result = rule->ShouldCollide;
                break;
            }
        }
    }
    
    
    return result;
}

internal bool32 HandleCollision(sim_entity *a, sim_entity *b)
{    
    bool32 stopsOnCollision = false;  
    
    
    stopsOnCollision = !(a->Type == ENTITY_TYPE_SWORD);
    
    // sim_entity *a = entity;
    // sim_entity *b = hitEntity; 
    if(a->Type > b->Type) // NOTE(Joey): sort by entity type; ensure we're always at top of double dispatch matrix
    {
        sim_entity *temp = a;
        a = b;
        b = temp;
    }
    
    if(a->Type == ENTITY_TYPE_MONSTER && b->Type == ENTITY_TYPE_SWORD)
    {
        if(a->HitPointMax > 0)
            --a->HitPointMax;
        // MakeEntityNonSpatial(b);
        // stopsOnCollision = true;
    }
    
    return stopsOnCollision;
}

internal void MoveEntity(game_state *gameState, sim_region *simRegion, sim_entity *entity, real32 dt, move_spec *moveSpec, vector3D acceleration)
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
    acceleration += { 0.0f, 0.0f, -9.81f };
            
    // update entity position (use velocity at begin of frame)
    vector3D oldPlayerPos = entity->Position;
    vector3D playerDelta = 0.5f * acceleration*Square(dt) + entity->Velocity*dt;
    vector3D newPlayerPos = oldPlayerPos + playerDelta;
    entity->Velocity += acceleration * dt;
    Assert(LengthSq(entity->Velocity) <= Square(simRegion->MaxEntityVelocity));
                
         
    // detect collisions         
    real32 distanceRemaining = entity->DistanceLimit;
    if(distanceRemaining == 0.0f)
        distanceRemaining  = 10000.0f;
        
    for(uint32 i = 0; i < 4; ++i)
    {
        real32 tMin = 1.0f;
        
        real32 playerDeltaLength = Length(playerDelta);                            
        if(playerDeltaLength > 0.0f)
        {
            if(playerDeltaLength > distanceRemaining)
                tMin = distanceRemaining / playerDeltaLength;
            
            vector3D wallNormal = {};                
            sim_entity *hitEntity = 0;
            
            vector3D desiredPosition = entity->Position + playerDelta;
            
          
            // NOTE(Joey): this is an optimization to avoid entering the loop when unnecesary
            if(!IsSet(entity, ENTITY_FLAG_NONSPATIAL))
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
                    if(ShouldCollide(gameState, entity, testEntity))
                    {                 
                       
                        vector3D minkowskiSum = { testEntity->Size.X  + entity->Size.X,
                                                  testEntity->Size.Y + entity->Size.Y,
                                                  testEntity->Size.Z + entity->Size.Z };
                        
                        vector3D minCorner = -0.5f*minkowskiSum;
                        vector3D maxCorner =  0.5f*minkowskiSum;

                        vector3D rel = entity->Position - testEntity->Position;
                 
                        if(TestWall(minCorner.X, rel.X, rel.Y, playerDelta.X, playerDelta.Y,
                                    &tMin, minCorner.Y, maxCorner.Y))
                        {
                            wallNormal = {-1, 0, 0};
                            hitEntity = testEntity;
                        }
                    
                        if(TestWall(maxCorner.X, rel.X, rel.Y, playerDelta.X, playerDelta.Y,
                                    &tMin, minCorner.Y, maxCorner.Y))
                        {
                            wallNormal = {1, 0, 0};
                            hitEntity = testEntity;
                        }
                    
                        if(TestWall(minCorner.Y, rel.Y, rel.X, playerDelta.Y, playerDelta.X,
                                    &tMin, minCorner.X, maxCorner.X))
                        {
                            wallNormal = {0, -1, 0};
                            hitEntity = testEntity;
                        }
                    
                        if(TestWall(maxCorner.Y, rel.Y, rel.X, playerDelta.Y, playerDelta.X,
                                    &tMin, minCorner.X, maxCorner.X))
                        {
                            wallNormal = {0, 1, 0};
                            hitEntity = testEntity;
                        }     
                    }            
                }   
            }       
            
            entity->Position += tMin*playerDelta;    
            distanceRemaining -= tMin*playerDeltaLength;
            // NOTE(Joey): special collision events, should something happen? (like stairs, cinematic start areas)
            if(hitEntity)
            {
                playerDelta = desiredPosition - entity->Position;
                              
                // collision dispatch
             
                bool32 stopsOnCollision = HandleCollision(entity, hitEntity);                
                if(stopsOnCollision)
                {
                    entity->Velocity = entity->Velocity - 1*InnerProduct(entity->Velocity, wallNormal)*wallNormal;
                    playerDelta = playerDelta - 1*InnerProduct(playerDelta, wallNormal)*wallNormal;
                }
                else
                {
                    AddCollisionRule(gameState, entity->StorageIndex, hitEntity->StorageIndex, false);
                }
            }
            else
                break;            
        }
    }           

    // hacky ground collision
    if(entity->Position.Z < 0)
    {
        entity->Position.Z = 0.0f;    
        entity->Velocity.Z = 0.0f;;
    }

    if(entity->DistanceLimit != 0.0f)
    {
        entity->DistanceLimit = distanceRemaining;
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