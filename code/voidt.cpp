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
#include "voidt.h"

#include "intrinsics.h"
#include "map.cpp"

internal void DrawRectangle(game_offscreen_buffer *buffer, vector2D min, vector2D max, real32 r, real32 g, real32 b)
{  
    int32 minX = RoundReal32ToInt32(min.X);
    int32 minY = RoundReal32ToInt32(min.Y);
    int32 maxX = RoundReal32ToInt32(max.X);
    int32 maxY = RoundReal32ToInt32(max.Y);
    
    if(minX < 0) minX = 0;
    if(minY < 0) minY = 0;
    if(maxX > buffer->Width) maxX = buffer->Width;
    if(maxY > buffer->Height) maxY = buffer->Height;
    
    // BIT PATTERN: 0x AA RR GG BB
    uint32 color = (RoundReal32ToUint32(r * 255.0f) << 16) |
                   (RoundReal32ToUint32(g * 255.0f) << 8) |
                   (RoundReal32ToUint32(b * 255.0f) << 0);
        
    uint8* row = (uint8*)buffer->Memory + minX * buffer->BytesPerPixel + minY * buffer->Pitch;
    
    for(int y = minY; y < maxY; ++y)
    {
        uint32* pixel = (uint32*)row;
        for(int x = minX; x < maxX; ++x)
        {            
            *pixel++ = color;        
        }          
        row += buffer->Pitch;
    }
    
}

internal void DrawBitmap(game_offscreen_buffer *buffer, loaded_bitmap *bitmap, real32 X, real32 Y, real32 alpha = 1.0f)
{
    // X -= (real32)alignX;
    // Y -= (real32)alignY;
    
    int32 minX = RoundReal32ToInt32(X);
    int32 minY = RoundReal32ToInt32(Y);
    int32 maxX = minX + bitmap->Width;
    int32 maxY = minY + bitmap->Height;
    
    int32 sourceOffsetX = 0; // fixing clipping
    int32 sourceOffsetY = 0;
    if(minX < 0) 
    {
        sourceOffsetX = -minX;
        minX = 0;
    }
    if(minY < 0)
    {
        sourceOffsetY = -minY;
        minY = 0;
    }
    if(maxX > buffer->Width)
    { 
        maxX = buffer->Width;
    }
    if(maxY > buffer->Height) 
    {
        maxY = buffer->Height;
    }
    
    
    uint32 *sourceRow = bitmap->Pixels + bitmap->Width*(bitmap->Height - 1); // start from top
    sourceRow += -sourceOffsetY*bitmap->Width + sourceOffsetX; // offset source access by however much we clipped 
    
    uint8 *destRow = (uint8*)buffer->Memory + minX * buffer->BytesPerPixel + minY * buffer->Pitch;
    
    for(int y = minY; y < maxY; ++y)
    {
        uint32 *dest = (uint32*)destRow;
        uint32 *source = (uint32*)sourceRow;
        for(int x = minX; x < maxX; ++x)
        {          
            real32 A = (real32)((*source >> 24) & 0xFF) / 255.0f;
            A *= alpha;
            real32 SR = (real32)((*source >> 16) & 0xFF);
            real32 SG = (real32)((*source >> 8) & 0xFF);
            real32 SB = (real32)((*source >> 0) & 0xFF);
            
            real32 DR = (real32)((*dest >> 16) & 0xFF);
            real32 DG = (real32)((*dest >> 8) & 0xFF);
            real32 DB = (real32)((*dest >> 0) & 0xFF);
    
            real32 R = (1.0f - A)*DR + A*SR;
            real32 G = (1.0f - A)*DG + A*SG;
            real32 B = (1.0f - A)*DB + A*SB;
            
            *dest = ((uint32)(R + 0.5) << 16) |
                    ((uint32)(G + 0.5) << 8) |
                    ((uint32)(B + 0.5) << 0);
    
            dest++; source++;
        }          
        destRow += buffer->Pitch;
        sourceRow -= bitmap->Width;
    }
    
    
}

void GameRender(game_offscreen_buffer *buffer, game_state *state)
{
    // int XOffset = 0;
    // int YOffset = 0;
    // render
    // uint8 *row = (uint8 *)screenBuffer->Memory; 
    // for(int Y = 0; Y < screenBuffer->Height; ++Y)
    // {
        // uint32 *Pixel = (uint32 *)row;
        // for(int X = 0; X < screenBuffer->Width; ++X)
        // {    
            // uint8 Blue  = (uint8)(X + xOffset);           // B
            // uint8 Green = (uint8)(Y + yOffset);           // G
            // uint8 Red   = (uint8)(X + yOffset + xOffset); // R
                        
            // *Pixel++ = (Red << 16) | (Green << 8) | Blue;
        // }
        // row += screenBuffer->Pitch;
    // }           

   
    
   
}



void GameOutputSound(game_sound_output_buffer *soundBuffer, game_state *gameState, int toneHz)
{
    // uint16 toneVolume = 3000;
    // uint16 wavePeriod = (uint16)(soundBuffer->SamplesPerSecond / toneHz);
    
    // int16 *sampleOut = soundBuffer->Samples;
    // for(int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
    // {
        // real32 sineValue = sinf((real32)gameState->tSine);
        // int16 sampleValue = (int16)(sineValue * toneVolume);
        // sampleValue = 0; // disable
        // *sampleOut++ = sampleValue;
        // *sampleOut++ = sampleValue;
        
        // gameState->tSine += 2.0f * Pi32 * 1.0f / wavePeriod;
    // }        
} 

// internal bool32 IsTileEmtpy(game_world *world, tile_map *world, int32 testTileX, int32 testTileY)
// {
    // bool32 empty = false;
    
    // if(world)
    // {
        // if(testTileX >= 0 && testTileX < world->TileCountX && testTileY >=0 && testTileY < world->TileCountY)
        // {
            // empty = GetTileValueUnchecked(world, world, testTileX, testTileY) == 0;       
        // }
    // }
    // return empty;    
    
    
// }




#pragma pack(push, 1)
struct bitmap_header
{
    uint16 FileType;
    uint32 FileSize;
    uint16 Reserved1;
    uint16 Reserved2;
    uint32 BitmapOffset;
    uint32 Size;
    int32 Width;
    int32 Height;
    uint16 Planes;
    uint16 BitsPerPixel;
    uint32 Compression;
    uint32 SizeOfBitmap;
    int32 HorzResolution;
    int32 VertResolution;
    uint32 ColorsUsed;
    uint32 ColorsImportant;
    
    uint32 RedMask;
    uint32 GreenMask;
    uint32 BlueMask;
    
};
#pragma pack(pop)

internal loaded_bitmap DEBUGLoadBMP(thread_context *thread, debug_platform_read_entire_file *readEntireFile, char *fileName)
{
    loaded_bitmap result = {};
    
    debug_read_file_result readResult = readEntireFile(thread, fileName);    
    if(readResult.ContentSize != 0)
    {
        bitmap_header *header = (bitmap_header *)readResult.Contents;
        uint32 *pixels = (uint32*)((uint8*)readResult.Contents + header->BitmapOffset);
        result.Pixels = pixels;
        result.Width= header->Width;
        result.Height = header->Height;
        
        Assert(header->Compression == 3);
        
        // NOTE(Joey): byte order in memory is AA BB GG RR (AA first in lowest memory address), bottom upper_bound
        // CPU reads it in as: RR GG BB AA (first reads AA, then BB)
        // we need AA first so switch AA to the back for each pixel
        // !!!actually: byte order is determined by the header itself with 3 masks for each individual color
        
        uint32 redMask = header->RedMask;
        uint32 greenMask = header->GreenMask;
        uint32 blueMask = header->BlueMask;
        uint32 alphaMask = ~(redMask | greenMask | blueMask);
        
        uint32 redShift = 0;
        uint32 greenShift = 0;
        uint32 blueShift = 0;
        uint32 alphaShift = 0;              
        
        bool32 found = FindLeastSignificantSetBit(&redShift, redMask);
        Assert(found);
        found = FindLeastSignificantSetBit(&greenShift, greenMask);
        Assert(found);
        found = FindLeastSignificantSetBit(&blueShift, blueMask);
        Assert(found);
        found = FindLeastSignificantSetBit(&alphaShift, alphaMask);
        Assert(found);
        
        redShift = 16 - (int32)redShift;
        greenShift = 8 - (int32)greenShift;
        blueShift = 0 - (int32)blueShift;
        alphaShift = 24 - (int32)alphaShift;
        
        uint32 *sourceDest = pixels;
        for(int32 y = 0; y < result.Height; ++y)
        {
            for(int x = 0; x < result.Width; ++x)
            {
                uint32 C = *sourceDest;
#if 0                
                *sourceDest++ = (((C >> alphaShift) & 0xFF) << 24) | 
                                (((C >> redShift  ) & 0xFF) << 16) | 
                                (((C >> greenShift) & 0xFF) << 8)  | 
                                (((C >> blueShift ) & 0xFF) << 0); 
#else
                *sourceDest++ = (RotateLeft(C & redMask, redShift) |
                                 RotateLeft(C & greenMask, greenShift) |
                                 RotateLeft(C & blueMask, blueShift) |
                                 RotateLeft(C & alphaMask, alphaShift));
#endif
            }            
        }
    }
    return result;
}


inline low_entity* GetLowEntity(game_state *gameState, uint32 lowIndex)
{
    low_entity *result = 0;
    if(lowIndex > 0 && lowIndex < gameState->LowEntityCount)
    {
        result = gameState->LowEntities + lowIndex;
    }
    return result;
}

inline vector2D GetCameraSpacePos(game_state *gameState, low_entity *lowEntity)
{
    world_difference diff = Subtract(gameState->World, &lowEntity->Position, &gameState->CameraPos);
    vector2D result = { diff.dX, diff.dY };
    return result;
}

internal high_entity* MakeEntityHighFrequency(game_state *gameState, low_entity *lowEntity, uint32 lowIndex, vector2D cameraSpacePos)
{
    high_entity *highEntity = 0;

    Assert(lowEntity->HighEntityIndex == 0);
    if(lowEntity->HighEntityIndex == 0) 
    {
        if(gameState->HighEntityCount < ArrayCount(gameState->HighEntities))
        {
            uint32 highIndex = gameState->HighEntityCount++;
            highEntity = &gameState->HighEntities[highIndex];
        
            
            highEntity->Position = cameraSpacePos;
            highEntity->Velocity = { 0, 0 };
            highEntity->ChunkZ = lowEntity->Position.ChunkZ;
            highEntity->FacingDirection = 0;
            highEntity->LowEntityIndex = lowIndex;
            
            lowEntity->HighEntityIndex = highIndex;
        }
        else
        {
            InvalidCodePath;
        }
    }
    
    return highEntity;
}


internal high_entity* MakeEntityHighFrequency(game_state *gameState, uint32 lowIndex)
{
    high_entity *highEntity = 0;
    
    low_entity *lowEntity = &gameState->LowEntities[lowIndex];
    if(lowEntity->HighEntityIndex)  
    {
        highEntity = gameState->HighEntities + lowEntity->HighEntityIndex;
    }
    else
    {
        vector2D cameraSpacePos = GetCameraSpacePos(gameState, lowEntity);
        highEntity = MakeEntityHighFrequency(gameState, lowEntity, lowIndex, cameraSpacePos);
    }    
    return highEntity;
}

inline game_entity ForceEntityIntoHigh(game_state *gameState, uint32 lowIndex)
{
    game_entity result = {};
    
    if(lowIndex > 0 && lowIndex < gameState->LowEntityCount)
    {   
        result.LowIndex = lowIndex;
        result.Low = gameState->LowEntities + lowIndex;
        result.High = MakeEntityHighFrequency(gameState, lowIndex);
    }
    return result;
}




internal void MakeEntityLowFrequency(game_state *gameState, uint32 lowIndex)
{    
    low_entity *lowEntity = &gameState->LowEntities[lowIndex];
    uint32 highIndex = lowEntity->HighEntityIndex;
    if(highIndex)
    {
        uint32 lastHighIndex = gameState->HighEntityCount = 1;
        if(highIndex != lastHighIndex)
        {
            // remove last high entity on the list to cleared spot
            high_entity *lastEntity = gameState->HighEntities + lastHighIndex; // access array by pointer => result pointer
            high_entity *delEntity = gameState->HighEntities + highIndex;
            
            *delEntity = *lastEntity;
            
            gameState->LowEntities[lastEntity->LowEntityIndex].HighEntityIndex = highIndex;
        }
        --gameState->HighEntityCount;
        lowEntity->HighEntityIndex = 0;
    }
}

inline void OffsetAndCheckFrequencyByArea(game_state *gameState, vector2D offset, rectangle2D highFrequencyBounds)
{
    for(uint32 entityIndex = 1; entityIndex < gameState->HighEntityCount;)
    {
        high_entity *high = gameState->HighEntities + entityIndex;
        
        high->Position += offset;        
        if(IsInRectangle(highFrequencyBounds, high->Position))
        {
            ++entityIndex;
        }
        else
        {
            MakeEntityLowFrequency(gameState, high->LowEntityIndex);
        }        
    }
}

struct add_low_entity_result
{
    low_entity *Low;
    uint32 LowIndex;
};

internal add_low_entity_result AddLowEntity(game_state *gameState, entity_type type, world_position *pos)
{
    Assert(gameState->LowEntityCount < ArrayCount(gameState->LowEntities));
    uint32 index = gameState->LowEntityCount++;
            
    low_entity *lowEntity = gameState->LowEntities + index;
    *lowEntity = {};    
    lowEntity->Type = type;
    
    if(pos)
    {
        lowEntity->Position = *pos;       
        // place entity in spatial hash-based world
        ChangeEntityLocation(&gameState->WorldArena, gameState->World, index, 0, pos);
    }
    
    add_low_entity_result result;
    result.Low = lowEntity;
    result.LowIndex = index;
    
    return result;
}


internal add_low_entity_result AddWall(game_state *gameState, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    world_position pos = ChunkPositionFromTilePosition(gameState->World, absTileX, absTileY, absTileZ);
    
    add_low_entity_result entity = AddLowEntity(gameState, ENTITY_TYPE_WALL, &pos); 
   
    entity.Low->Height = gameState->World->TileSideInMeters;
    entity.Low->Width  = gameState->World->TileSideInMeters;
    entity.Low->Collides = true;
    
    return entity;
}

internal add_low_entity_result AddPlayer(game_state *gameState)
{
    world_position pos = gameState->CameraPos;
    add_low_entity_result entity = AddLowEntity(gameState, ENTITY_TYPE_PLAYER, &pos);
    
    entity.Low->Position = gameState->CameraPos;
    entity.Low->Collides = true;

    entity.Low->Height = 0.5f;
    entity.Low->Width = 1.0f;
    
    if(gameState->CameraFollowingEntityIndex == 0)
    {
        gameState->CameraFollowingEntityIndex = entity.LowIndex;
    }    
        
    return entity;
}

internal add_low_entity_result AddMonster(game_state *gameState, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    world_position pos = ChunkPositionFromTilePosition(gameState->World, absTileX, absTileY, absTileZ);    
    add_low_entity_result entity = AddLowEntity(gameState, ENTITY_TYPE_MONSTER, &pos);
    
    entity.Low->Height = 0.5f;
    entity.Low->Width = 1.0f;
    entity.Low->Collides = true;
    
    return entity;
}

internal add_low_entity_result AddFamiliar(game_state *gameState, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    world_position pos = ChunkPositionFromTilePosition(gameState->World, absTileX, absTileY, absTileZ);    
    add_low_entity_result entity = AddLowEntity(gameState, ENTITY_TYPE_FAMILIAR, &pos);
    
    entity.Low->Height = 0.5f;
    entity.Low->Width = 1.0f;
    entity.Low->Collides = false;
    
    return entity;
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



internal void MoveEntity(game_state *gameState, game_entity entity, real32 dt, vector2D acceleration)
{
    game_world *world = gameState->World;
    
    real32 accelerationLength = LengthSq(acceleration); // get squared length, still valid to check length > 1.0 and much cheaper (no sqrt)
    if(accelerationLength > 1.0f)
    {
        acceleration *= 1.0f / SquareRoot(accelerationLength);
    }
    
    real32 speed = 50.0f;
    acceleration *= speed;
            
    // add friction (approximation) to acceleration
    acceleration += -8.0f*entity.High->Velocity;
            
    // update entity position (use velocity at begin of frame)
    vector2D oldPlayerPos = entity.High->Position;
    vector2D playerDelta = 0.5f * acceleration*Square(dt) + entity.High->Velocity*dt;
    vector2D newPlayerPos = oldPlayerPos + playerDelta;
    entity.High->Velocity += acceleration * dt;
            
         
    // detect collisions         
    // real32 tRemaining = 1.0f;      
    for(uint32 i = 0; i < 4; ++i)
    {
        real32 tMin = 1.0f;
        vector2D wallNormal = {};                
        uint32 hitHighEntityIndex = 0;
        
        vector2D desiredPosition = entity.High->Position + playerDelta;
        
        for(uint32 entityIndex = 1; entityIndex < gameState->HighEntityCount; ++entityIndex)
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
            
            if(entityIndex != entity.Low->HighEntityIndex)
            {                
                game_entity testEntity;
                testEntity.High = gameState->HighEntities + entityIndex;
                testEntity.Low = gameState->LowEntities +  testEntity.High->LowEntityIndex;
                testEntity.LowIndex = testEntity.High->LowEntityIndex;

                if(testEntity.Low->Collides)
                {
                    real32 diameterW = testEntity.Low->Width  + entity.Low->Width;
                    real32 diameterH = testEntity.Low->Height + entity.Low->Height;
                    vector2D minCorner = -0.5f*vector2D{diameterW, diameterH};
                    vector2D maxCorner = 0.5f*vector2D{diameterW, diameterH};

                    vector2D rel = entity.High->Position - testEntity.High->Position;
             
                    if(TestWall(minCorner.X, rel.X, rel.Y, playerDelta.X, playerDelta.Y,
                                &tMin, minCorner.Y, maxCorner.Y))
                    {
                        wallNormal = {-1, 0};
                        hitHighEntityIndex = entityIndex;
                    }
                
                    if(TestWall(maxCorner.X, rel.X, rel.Y, playerDelta.X, playerDelta.Y,
                                &tMin, minCorner.Y, maxCorner.Y))
                    {
                        wallNormal = {1, 0};
                        hitHighEntityIndex = entityIndex;
                    }
                
                    if(TestWall(minCorner.Y, rel.Y, rel.X, playerDelta.Y, playerDelta.X,
                                &tMin, minCorner.X, maxCorner.X))
                    {
                        wallNormal = {0, -1};
                        hitHighEntityIndex = entityIndex;
                    }
                
                    if(TestWall(maxCorner.Y, rel.Y, rel.X, playerDelta.Y, playerDelta.X,
                                &tMin, minCorner.X, maxCorner.X))
                    {
                        wallNormal = {0, 1};
                        hitHighEntityIndex = entityIndex;
                    }     
                }
            }            
        }    
        entity.High->Position += tMin*playerDelta;    
        // special collision events, should something happen? (like stairs, cinematic start areas)
        if(hitHighEntityIndex)
        {
            entity.High->Velocity = entity.High->Velocity - 1*InnerProduct(entity.High->Velocity, wallNormal)*wallNormal;
            playerDelta = desiredPosition - entity.High->Position;
            playerDelta = playerDelta - 1*InnerProduct(playerDelta, wallNormal)*wallNormal;
            // tRemaining -= tMin*tRemaining;
            
            high_entity *hitHigh = gameState->HighEntities + hitHighEntityIndex;
            low_entity  *hitLow  = gameState->LowEntities  + hitHigh->LowEntityIndex;
            // entity.High->AbsTileZ += hitLow->dAbsTileZ;
        }
        else
            break;
    }                         
    
    // determine new facing direction
    if(entity.High->Velocity.X == 0.0f && entity.High->Velocity.Y == 0.0f)
    {
        // NOTE(Joey): leave facing direction to earlier set facing direction
    }
    else if(Absolute(entity.High->Velocity.X) > Absolute(entity.High->Velocity.Y))
    {
        entity.High->FacingDirection = entity.High->Velocity.X > 0 ? 0 : 2;            
    }
    else
    {
        entity.High->FacingDirection = entity.High->Velocity.Y > 0 ? 1 : 3;
    }       
    world_position newPos = MapIntoChunkSpace(gameState->World, gameState->CameraPos, entity.High->Position);
    ChangeEntityLocation(&gameState->WorldArena, gameState->World, entity.LowIndex, &entity.Low->Position, &newPos);
    entity.Low->Position = newPos;
}

internal void SetCamera(game_state *gameState, world_position newCameraPos)
{
    // determine how far camera moved, and update all entities back to proper camera space
    game_world *world = gameState->World;
    world_difference dCameraPos = Subtract(world, &newCameraPos, &gameState->CameraPos);
    gameState->CameraPos = newCameraPos;
            
    vector2D entityOffsetPerFrame = {};
    entityOffsetPerFrame = { -dCameraPos.dX, -dCameraPos.dY };
    
    /* NOTE(Joey)
      
      If camera position gets updated, loop over all high
      frequency entities and remove those from the list 
      that are now out of the high frequency range.
      
      Add entities from the low frequency list onto the
      high frequency list that have now entered the 
      pre-defined high frequency range.
    
      The high frequency range is a screen region of 
      3x3 surrounding the center camera screen pos.    
    */
    uint32 tileSpanX = 17*3;
    uint32 tileSpanY =  9*3;
    rectangle2D cameraBounds = RectCenterDim(vector2D { 0, 0 }, world->TileSideInMeters*vector2D { (real32)tileSpanX, (real32)tileSpanY });                                                 
   
    OffsetAndCheckFrequencyByArea(gameState, entityOffsetPerFrame, cameraBounds);
    
    // TODO(Joey): move entities into high set here
    world_position minChunkPos = MapIntoChunkSpace(world, newCameraPos, GetMinCorner(cameraBounds));
    world_position maxChunkPos = MapIntoChunkSpace(world, newCameraPos, GetMaxCorner(cameraBounds));
    
    for(int32 chunkY = minChunkPos.ChunkY; chunkY <= maxChunkPos.ChunkY; ++chunkY)
    {
        for(int32 chunkX = minChunkPos.ChunkX; chunkX <= maxChunkPos.ChunkX; ++chunkX)
        {
            world_chunk *chunk = GetWorldChunk(world, chunkX, chunkY, newCameraPos.ChunkZ);
            if(chunk)
            {
                for(world_entity_block *block = &chunk->FirstBlock; block; block = block->Next)
                {
                    for(uint32 entityIndexIndex = 0; entityIndexIndex < block->EntityCount; ++entityIndexIndex)
                    {
                        uint32 lowEntityIndex = block->LowEntityIndex[entityIndexIndex];
                        low_entity *low = gameState->LowEntities + lowEntityIndex;                        
                        if(low->HighEntityIndex == 0)
                        {
                            vector2D cameraSpacePos = GetCameraSpacePos(gameState, low);
                            if(IsInRectangle(cameraBounds, cameraSpacePos))
                            {
                                MakeEntityHighFrequency(gameState, low, lowEntityIndex, cameraSpacePos);
                            }
                        }
                    }
                }
            }
        }
    }
}

inline game_entity EntityFromHighIndex(game_state *gameState, uint32 highEntityIndex)
{
    game_entity result = {};
    
    if(highEntityIndex)
    {
        Assert(highEntityIndex < ArrayCount(gameState->HighEntities));
        result.High = gameState->HighEntities + highEntityIndex;
        result.LowIndex = result.High->LowEntityIndex;
        result.Low = gameState->LowEntities + result.LowIndex;
    }
    
    return result;
}

internal void UpdateFamiliar(game_state *gameState, game_entity entity, real32 dt)
{
    game_entity closestPlayer = {};
    real32 closestPlayerDSq = 100.0f; // NOTE(Joey): don't search for more than 10 meters
    for(uint32 highEntityIndex = 1; highEntityIndex < gameState->HighEntityCount; ++highEntityIndex)
    {
        game_entity testEntity = EntityFromHighIndex(gameState, highEntityIndex);
        
        if(testEntity.Low->Type == ENTITY_TYPE_PLAYER)
        {
            real32 testDSq = LengthSq(testEntity.High->Position - entity.High->Position);
            if(testDSq <= closestPlayerDSq)
            {
                closestPlayer = testEntity;
                closestPlayerDSq = testDSq;
            }
        }
    }
    
    vector2D acceleration = {};
    if(closestPlayer.High && closestPlayerDSq > 0.1f)
    {
        real32 speed = 0.5f;
        real32 oneOverLength = speed / SquareRoot(closestPlayerDSq);
        acceleration = oneOverLength * (closestPlayer.High->Position - entity.High->Position);
        
    }
    MoveEntity(gameState, entity, dt, acceleration);
}

internal void UpdateMonster(game_state *gameState, game_entity entity, real32 dt)
{
    
}

inline void PushPiece(entity_visible_piece_group *group, loaded_bitmap *bitmap, vector2D offset, real32 offsetZ, vector2D align, real32 alpha = 1.0f)
{
    Assert(group->PieceCount < ArrayCount(group->Pieces));
    
    entity_visible_piece *piece = &group->Pieces[group->PieceCount++];
    piece->Bitmap = bitmap;
    piece->Offset = offset - align;
    piece->OffsetZ = offsetZ;
    piece->Alpha = alpha;    
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize);      
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {              
        // NOTE(Joey): Reserve entity slot 0 for the null entity
        AddLowEntity(gameState, ENTITY_TYPE_NULL, 0);
        gameState->HighEntityCount = 1; // reserve spot 0 for null entity

        gameState->BackDrop = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_background.bmp");
        gameState->Shadow = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_shadow.bmp");
        gameState->Tree = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test2/tree00.bmp");
        
        gameState->HeroBitmaps[0].Head = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_right_head.bmp");
        gameState->HeroBitmaps[0].Torso = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_right_torso.bmp");
        gameState->HeroBitmaps[0].Cape = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_right_cape.bmp");
        gameState->HeroBitmaps[0].Align = { 72, 182 };
        
        gameState->HeroBitmaps[1].Head = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_back_head.bmp");
        gameState->HeroBitmaps[1].Torso = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_back_torso.bmp");
        gameState->HeroBitmaps[1].Cape = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_back_cape.bmp");
        gameState->HeroBitmaps[1].Align = { 72, 182 };
        
        gameState->HeroBitmaps[2].Head = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_left_head.bmp");
        gameState->HeroBitmaps[2].Torso = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_left_torso.bmp");
        gameState->HeroBitmaps[2].Cape = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_left_cape.bmp");
        gameState->HeroBitmaps[2].Align = { 72, 182 };
        
        gameState->HeroBitmaps[3].Head = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_front_head.bmp");
        gameState->HeroBitmaps[3].Torso = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_front_torso.bmp");
        gameState->HeroBitmaps[3].Cape = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_front_cape.bmp");
        gameState->HeroBitmaps[3].Align = { 72, 182 };
       
        //////////////////////////////////////////////////////////
        //       WORLD GENERATION 
        //////////////////////////////////////////////////////////            
        InitializeArena(&gameState->WorldArena, memory->PermanentStorageSize - sizeof(game_state), (uint8*)memory->PermanentStorage + sizeof(game_state));
        
        gameState->World = PushStruct(&gameState->WorldArena, game_world);
        game_world *world = gameState->World;
        // world->TileMap = PushStruct(&gameState->WorldArena, tile_map);
                
        // tile_map *world = world->TileMap;
        InitializeWorld(world, 1.4f);
        
        uint32 tilesPerWidth = 17;
        uint32 tilesPerHeight = 9;       
        uint32 screenBaseX = 0;
        uint32 screenBaseY = 0;
        uint32 screenBaseZ = 0;
        uint32 screenX = screenBaseX;        
        uint32 screenY = screenBaseY;
        uint32 absTileZ = screenBaseZ;
        
        bool doorTop = false;
        bool doorBottom = false;
        bool doorLeft = false;
        bool doorRight = false;
        bool doorUp  = false;
        bool doorDown = false;
        for(uint32 screenIndex = 0; screenIndex < 2000; ++screenIndex)
        {
            uint32 randomChoice = 0;
            // if(doorUp || doorDown)
                randomChoice = (885447 * screenIndex + (screenIndex + 7) * 7637 / 937) % 2;
            // else
                // randomChoice = (885447 * screenIndex + (screenIndex + 7) * 7637 / 937) % 3;

            bool createdZDoor = false;
            if(randomChoice == 2)
            {
                createdZDoor = true;
                if (absTileZ == screenBaseZ)
                    doorUp = true;
                else
                    doorDown = true;
            }                
            else if (randomChoice == 1)
                doorRight = true;
            else
                doorTop = true;
            
            
            for(uint32 tileY = 0; tileY < tilesPerHeight; ++tileY)
            {
                for(uint32 tileX = 0; tileX< tilesPerWidth; ++tileX)
                {
                    uint32 absTileX = screenX*tilesPerWidth + tileX;
                    uint32 absTileY = screenY*tilesPerHeight + tileY;
                    
                    uint32 tileValue = 1;
                    if((tileX == 0) && (!doorLeft || (tileY != (tilesPerHeight / 2))))
                    {
                        tileValue = 2;
                    }
                    if((tileX == (tilesPerWidth - 1)) && (!doorRight || (tileY != (tilesPerHeight / 2))))
                    {
                        tileValue = 2;
                    }
                    if((tileY == 0) && (!doorBottom || (tileX != (tilesPerWidth / 2))))
                    {
                        tileValue = 2;
                    }
                    if((tileY == (tilesPerHeight - 1)) && (!doorTop || (tileX != (tilesPerWidth / 2))))
                    {
                        tileValue = 2;
                    }
                    
                    if((tileX == 10) && (tileY == 6))
                    {
                        if(doorUp) 
                        {
                            tileValue = 3;
                        }
                        if(doorDown)
                        {
                            tileValue = 4;
                        }
                    }                      
                                        
                    // SetTileValue(&gameState->WorldArena, world->TileMap, absTileX, absTileY, absTileZ, tileValue);
                    
                    if(tileValue == 2)
                        AddWall(gameState, absTileX, absTileY, absTileZ);
                    // int entityIndex = AddEntity(gameState);
                }
            }
            
                      
            doorLeft = doorRight;
            doorBottom = doorTop;
            
            if(createdZDoor)
            {
                doorDown = !doorDown;
                doorUp = !doorUp;
            }
            else
            {
                doorUp = false;
                doorDown = false;
            }
  
            
            doorRight = false;
            doorTop = false;
            
            if(randomChoice == 2)
            {
                if(absTileZ == screenBaseZ) 
                {
                    absTileZ = screenBaseZ + 1;
                }
                else 
                {
                    absTileZ = screenBaseZ;
                }
            }
            else if(randomChoice == 1)
            {
                screenX += 1;
            }
            else
            {
                screenY += 1;
            }
        }
        
        // set camera and setup initial entity frequency lists
        world_position newCameraPos = {};
        uint32 cameraTileX = screenBaseX*tilesPerWidth + 17/2;
        uint32 cameraTileY = screenBaseY*tilesPerHeight + 9/2;
        uint32 cameraTileZ = screenBaseZ;
        newCameraPos = ChunkPositionFromTilePosition(gameState->World,
                                                     cameraTileX,
                                                     cameraTileY, 
                                                     cameraTileZ);
        
        AddMonster(gameState, cameraTileX + 2, cameraTileY + 2, cameraTileZ);
        AddFamiliar(gameState, cameraTileX - 2, cameraTileY + 2, cameraTileZ);
        
        
        SetCamera(gameState, newCameraPos);

        memory->IsInitialized = true;
    }            
    
  
    
    //////////////////////////////////////////////////////////
    //       GAME INPUT 
    //////////////////////////////////////////////////////////   
    for(int controllerIndex = 0; controllerIndex < ArrayCount(input->Controllers); ++controllerIndex)
    {
        game_controller_input *controller = GetController(input, controllerIndex);
        uint32 lowIndex = gameState->PlayerControllerIndex[controllerIndex];
        if(lowIndex == 0)
        {
            if(controller->Start.EndedDown)
            {
                add_low_entity_result entity = AddPlayer(gameState);
                gameState->PlayerControllerIndex[controllerIndex] = entity.LowIndex;                
            }
        }
        else
        {
            game_entity controllingEntity = ForceEntityIntoHigh(gameState, lowIndex);
            vector2D dAcceleration = {};
            if(controller->IsAnalog)
            {
                // analog movement tuning
                dAcceleration = { controller->StickAverageX, controller->StickAverageY };
            }
            else
            {
                // digital movement tuning
                if(controller->MoveUp.EndedDown)
                {
                    dAcceleration.Y =  1.0f;
                }
                if(controller->MoveDown.EndedDown)
                {
                    dAcceleration.Y = -1.0f;
                }
                if(controller->MoveLeft.EndedDown)
                {
                    dAcceleration.X = -1.0f;
                }
                if(controller->MoveRight.EndedDown)
                {
                    dAcceleration.X =  1.0f;
                }                                
            }
            
            if(controller->ActionUp.EndedDown)
                controllingEntity.High->dZ = 3.0f;
            
            MoveEntity(gameState, controllingEntity, input->dtPerFrame, dAcceleration);
        }
    }
    
    
    
    //////////////////////////////////////////////////////////
    //       UPDATE CAMERA
    //////////////////////////////////////////////////////////   
    // move camera based on whichever entity we follow
    game_world *world = gameState->World;
    
    // vector2D entityOffsetPerFrame = {};
    game_entity cameraFollowingEntity = ForceEntityIntoHigh(gameState, gameState->CameraFollowingEntityIndex);
    if(cameraFollowingEntity.High)
    {
        world_position newCameraP = gameState->CameraPos;
        
        // newCameraP.ChunkZ = cameraFollowingEntity.Low->Position.ChunkZ;
    
        // if(cameraFollowingEntity.High->Position.X > 9.0f*world->TileSideInMeters)
            // newCameraP.ChunkX += 17;
        // if(cameraFollowingEntity.High->Position.X < -9.0f*world->TileSideInMeters)
            // newCameraP.ChunkX -= 17;
        // if(cameraFollowingEntity.High->Position.Y > 5.0f*world->TileSideInMeters)
            // newCameraP.ChunkY += 9;
        // if(cameraFollowingEntity.High->Position.Y < -5.0f*world->TileSideInMeters)
            // newCameraP.ChunkY -= 9;
        
        newCameraP = cameraFollowingEntity.Low->Position;
        
        SetCamera(gameState, newCameraP);
    }   
    
                        
      
        
    // render to screen memory      
    int32  TileSideInPixels= 60;
    real32 MetersToPixels = TileSideInPixels / world->TileSideInMeters;       
        
    real32 LowerLeftX = -TileSideInPixels / 2.0f;
    real32 LowerLeftY = screenBuffer->Height;
    
    vector2D screenCenter = { 0.5f*(real32)screenBuffer->Width, 0.5f*(real32)screenBuffer->Height };

    DrawRectangle(screenBuffer, { 0.0f, 0.0f }, {(real32)screenBuffer->Width, (real32)screenBuffer->Height }, 0.5f, 0.5f, 0.5f);
    // DrawBitmap(screenBuffer, &gameState->BackDrop, 0.0f, 0.0f);
    

        
    //////////////////////////////////////////////////////////
    //       RENDER 
    //////////////////////////////////////////////////////////        
    entity_visible_piece_group pieceGroup;
    for(uint32 entityIndex = 1; entityIndex < gameState->HighEntityCount; ++entityIndex)
    {
        pieceGroup.PieceCount = 0;
        
        high_entity *highEntity = &gameState->HighEntities[entityIndex];            
        low_entity *lowEntity = gameState->LowEntities + highEntity->LowEntityIndex;             
        
        // update entities
        game_entity entity;
        entity.LowIndex = highEntity->LowEntityIndex;
        entity.Low = lowEntity;
        entity.High = highEntity;
        
        
        real32 alpha = 1.0f - highEntity->Z;
        if(alpha < 0)
            alpha = 0.0f;
        
                
        real32 dt = input->dtPerFrame;
        uint32 facingDirection = highEntity->FacingDirection;
        
        hero_bitmaps *heroBitmaps = &gameState->HeroBitmaps[facingDirection];
        
        switch(lowEntity->Type)
        {
            case ENTITY_TYPE_PLAYER:
            {
                // DrawRectangle(screenBuffer, { playerLeft, playerTop }, { playerLeft + MetersToPixels*lowEntity->Width, playerTop + MetersToPixels*lowEntity->Height}, playerR, playerG, playerB);
                ;
                
                PushPiece(&pieceGroup, &gameState->Shadow, { 0, 0 }, 0, heroBitmaps->Align, alpha);            
                PushPiece(&pieceGroup, &heroBitmaps->Torso, { 0, 0 }, 0, heroBitmaps->Align);
                PushPiece(&pieceGroup, &heroBitmaps->Cape, { 0, 0}, 0, heroBitmaps->Align);
                PushPiece(&pieceGroup, &heroBitmaps->Head, { 0, 0 }, 0, heroBitmaps->Align);        
            } break;        
            case ENTITY_TYPE_WALL:
            {
                PushPiece(&pieceGroup, &gameState->Tree, { 0, 0 }, 0, { 40, 80 });
                // DrawRectangle(screenBuffer, { playerLeft, playerTop }, { playerLeft + MetersToPixels*lowEntity->Width, playerTop + MetersToPixels*lowEntity->Height}, playerR, playerG, playerB);
            } break;        
            case ENTITY_TYPE_MONSTER:
            {
                UpdateMonster(gameState, entity, dt);
                PushPiece(&pieceGroup, &gameState->Shadow, { 0, 0 }, 0, heroBitmaps->Align, alpha);            
                PushPiece(&pieceGroup, &heroBitmaps->Torso, { 0, 0 }, 0, heroBitmaps->Align);  
            } break;
            case ENTITY_TYPE_FAMILIAR:
            {
                UpdateFamiliar(gameState, entity, dt);
                PushPiece(&pieceGroup, &gameState->Shadow, { 0, 0 }, 0, heroBitmaps->Align, alpha);            
                PushPiece(&pieceGroup, &heroBitmaps->Head, { 0, 0 }, 0, heroBitmaps->Align);        
            } break;
            default:
                InvalidCodePath;
                break;
        }
        
        // jump
        real32 ddZ = -9.81f;
        highEntity->Z +=  0.5f*ddZ*Square(dt) + highEntity->dZ*dt;
        highEntity->dZ = highEntity->dZ + ddZ*dt;
        if(highEntity->Z < 0)
            highEntity->Z = 0.0f;
        
        real32 entityGroundPointX = screenCenter.X + MetersToPixels*highEntity->Position.X;
        real32 entityGroundPointY = screenCenter.Y - MetersToPixels*highEntity->Position.Y;
        real32 Z = -MetersToPixels*highEntity->Z;
        // real32 playerLeft = playerGroundPointX - 0.5f*MetersToPixels*lowEntity->Width;
        // real32 playerTop = playerGroundPointY - 0.5f*MetersToPixels*lowEntity->Height;         
        
        for(uint32 pieceIndex = 0; pieceIndex < pieceGroup.PieceCount; ++pieceIndex)
        {
            entity_visible_piece *piece = &pieceGroup.Pieces[pieceIndex];
            DrawBitmap(screenBuffer, piece->Bitmap, entityGroundPointX + piece->Offset.X, entityGroundPointY + piece->Offset.Y + piece->OffsetZ + Z, piece->Alpha);
        }
                  
    }
    

    // GameRender(screenBuffer, gameState);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, 412);
}
