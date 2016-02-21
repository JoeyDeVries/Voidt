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

internal void DrawBitmap(game_offscreen_buffer *buffer, loaded_bitmap *bitmap, real32 X, real32 Y, int32 alignX = 0, int32 alignY = 0, real32 alpha = 1.0f)
{
    X -= (real32)alignX;
    Y -= (real32)alignY;
    
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

// internal bool32 IsTileEmtpy(game_world *world, tile_map *tileMap, int32 testTileX, int32 testTileY)
// {
    // bool32 empty = false;
    
    // if(tileMap)
    // {
        // if(testTileX >= 0 && testTileX < world->TileCountX && testTileY >=0 && testTileY < world->TileCountY)
        // {
            // empty = GetTileValueUnchecked(world, tileMap, testTileX, testTileY) == 0;       
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

internal high_entity* MakeEntityHighFrequency(game_state *gameState, uint32 lowIndex)
{
    high_entity *highEntity = 0;
    low_entity *lowEntity = &gameState->LowEntities[lowIndex];
    if(lowEntity->HighEntityIndex)  
    {
        highEntity = gameState->HighEntities + lowEntity->HighEntityIndex;
        return highEntity;
    }
    else
    {
        if(gameState->HighEntityCount < ArrayCount(gameState->HighEntities))
        {
            uint32 highIndex = gameState->HighEntityCount++;
            highEntity = &gameState->HighEntities[highIndex];
        
            tile_map_difference diff = Subtract(gameState->World->TileMap, &lowEntity->Position, &gameState->CameraPos);
            highEntity->Position = { diff.dX, diff.dY };
            highEntity->Velocity = { 0, 0 };
            highEntity->AbsTileZ = lowEntity->Position.AbsTileZ;
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

inline game_entity GetHighEntity(game_state *gameState, uint32 lowIndex)
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

inline void OffsetAndCheckFrequencyByArea(game_state *gameState, vector2D offset, rectangle2D cameraBounds)
{
    for(uint32 entityIndex = 1; entityIndex < gameState->HighEntityCount;)
    {
        high_entity *high = gameState->HighEntities + entityIndex;
        
        high->Position += offset;        
        if(IsInRectangle(cameraBounds, high->Position))
        {
            ++entityIndex;
        }
        else
        {
            MakeEntityLowFrequency(gameState, high->LowEntityIndex);
        }        
    }
}

internal uint32 AddLowEntity(game_state *gameState, entity_type type)
{
    Assert(gameState->LowEntityCount < ArrayCount(gameState->LowEntities));
    uint32 index = gameState->LowEntityCount++;
            
    gameState->LowEntities[index] = {};    
    gameState->LowEntities[index].Type = type;
    
    return index;
}


internal uint32 AddWall(game_state *gameState, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    uint32 entityIndex = AddLowEntity(gameState, ENTITY_TYPE_WALL);
    low_entity *entity = GetLowEntity(gameState, entityIndex);
    
    entity->Position.AbsTileX = absTileX;
    entity->Position.AbsTileY = absTileY;
    entity->Position.AbsTileZ = absTileZ;
    entity->Height = gameState->World->TileMap->TileSideInMeters;
    entity->Width  = gameState->World->TileMap->TileSideInMeters;
    entity->Collides = true;
    
    return entityIndex;
}

internal uint32 AddPlayer(game_state *gameState)
{
    uint32 entityIndex = AddLowEntity(gameState, ENTITY_TYPE_PLAYER);
    low_entity *entity = GetLowEntity(gameState, entityIndex);
    
    entity->Position = gameState->CameraPos;
    // entity->Position.AbsTileX = 1;
    // entity->Position.AbsTileY = 3;
    entity->Collides = true;
    
    entity->Height = 0.5f;
    entity->Width = 1.0f;
    
    if(gameState->CameraFollowingEntityIndex == 0)
    {
        gameState->CameraFollowingEntityIndex = entityIndex;
    }    
    return entityIndex;
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



internal void MovePlayer(game_state *gameState, game_entity player, real32 dt, vector2D acceleration)
{
    tile_map *tileMap = gameState->World->TileMap;
    
    real32 accelerationLength = LengthSq(acceleration); // get squared length, still valid to check length > 1.0 and much cheaper (no sqrt)
    if(accelerationLength > 1.0f)
    {
        acceleration *= 1.0f / SquareRoot(accelerationLength);
    }
    
    real32 speed = 50.0f;
    acceleration *= speed;
            
    // add friction (approximation) to acceleration
    acceleration += -8.0f*player.High->Velocity;
            
    // update player position (use velocity at begin of frame)
    vector2D oldPlayerPos = player.High->Position;
    vector2D playerDelta = 0.5f * acceleration*Square(dt) + player.High->Velocity*dt;
    vector2D newPlayerPos = oldPlayerPos + playerDelta;
    player.High->Velocity += acceleration * dt;
            
         
    // detect collisions
    

    /*
    uint32 minTileX = Minimum(oldPlayerPos.AbsTileX, newPlayerPos.AbsTileX);
    uint32 minTileY = Minimum(oldPlayerPos.AbsTileY, newPlayerPos.AbsTileY);
    uint32 maxTileX = Maximum(oldPlayerPos.AbsTileX, newPlayerPos.AbsTileX);
    uint32 maxTileY = Maximum(oldPlayerPos.AbsTileY, newPlayerPos.AbsTileY);

    // get width/height of entity in tiles (also base tiles to check on minkowski sum)
    uint32 entityTileWidth = CeilReal32ToInt32(player.Low->Width / tileMap->TileSideInMeters);
    uint32 entityTileHeight = CeilReal32ToInt32(player.Low->Height / tileMap->TileSideInMeters);
    
    minTileX -= entityTileWidth;
    minTileY -= entityTileHeight;
    maxTileX += entityTileWidth;
    maxTileY += entityTileHeight;
    Assert(maxTileX - minTileX < 32);
    Assert(maxTileY - minTileY < 32);
    
    uint32 absTileZ = player.Low->Position.AbsTileZ;
    */

       

    // real32 tRemaining = 1.0f;      
    for(uint32 i = 0; i < 4; ++i)
    {
        real32 tMin = 1.0f;
        vector2D wallNormal = {};                
        uint32 hitHighEntityIndex = 0;
        
        vector2D desiredPosition = player.High->Position + playerDelta;
        
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
            
            if(entityIndex != player.Low->HighEntityIndex)
            {                
                game_entity testEntity;
                testEntity.High = gameState->HighEntities + entityIndex;
                testEntity.Low = gameState->LowEntities +  testEntity.High->LowEntityIndex;
                testEntity.LowIndex = testEntity.High->LowEntityIndex;

                if(testEntity.Low->Collides)
                {
                    real32 diameterW = testEntity.Low->Width  + player.Low->Width;
                    real32 diameterH = testEntity.Low->Height + player.Low->Height;
                    vector2D minCorner = -0.5f*vector2D{diameterW, diameterH};
                    vector2D maxCorner = 0.5f*vector2D{diameterW, diameterH};

                    vector2D rel = player.High->Position - testEntity.High->Position;
             
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
        player.High->Position += tMin*playerDelta;    
        // special collision events, should something happen? (like stairs, cinematic start areas)
        if(hitHighEntityIndex)
        {
            player.High->Velocity = player.High->Velocity - 1*InnerProduct(player.High->Velocity, wallNormal)*wallNormal;
            playerDelta = desiredPosition - player.High->Position;
            playerDelta = playerDelta - 1*InnerProduct(playerDelta, wallNormal)*wallNormal;
            // tRemaining -= tMin*tRemaining;
            
            high_entity *hitHigh = gameState->HighEntities + hitHighEntityIndex;
            low_entity  *hitLow  = gameState->LowEntities  + hitHigh->LowEntityIndex;
            player.High->AbsTileZ += hitLow->dAbsTileZ;
        }
        else
            break;
    }                         
    
    // determine new facing direction
    if(player.High->Velocity.X == 0.0f && player.High->Velocity.Y == 0.0f)
    {
        // NOTE(Joey): leave facing direction to earlier set facing direction
    }
    else if(Absolute(player.High->Velocity.X) > Absolute(player.High->Velocity.Y))
    {
        player.High->FacingDirection = player.High->Velocity.X > 0 ? 0 : 2;            
    }
    else
    {
        player.High->FacingDirection = player.High->Velocity.Y > 0 ? 1 : 3;
    }       
    player.Low->Position = MapIntoTileSpace(gameState->World->TileMap, gameState->CameraPos, player.High->Position);
}

internal void SetCamera(game_state *gameState, tile_map_position newCameraPos)
{
    // determine how far camera moved, and update all entities back to proper camera space
    tile_map *tileMap = gameState->World->TileMap;
    tile_map_difference dCameraPos = Subtract(tileMap, &newCameraPos, &gameState->CameraPos);
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
    rectangle2D cameraBounds = RectCenterDim(vector2D { 0, 0 }, tileMap->TileSideInMeters*vector2D { (real32)tileSpanX, (real32)tileSpanY });                                                 
   
    OffsetAndCheckFrequencyByArea(gameState, entityOffsetPerFrame, cameraBounds);
    
    // TODO(Joey): move entities into high set here
    int32 minTileX = newCameraPos.AbsTileX - tileSpanX/2;
    int32 maxTileX = newCameraPos.AbsTileX + tileSpanX/2;
    int32 minTileY = newCameraPos.AbsTileY - tileSpanY/2;
    int32 maxTileY = newCameraPos.AbsTileY + tileSpanY/2;
    for(uint32 entityIndex = 1; entityIndex < gameState->LowEntityCount; ++entityIndex)
    {
            low_entity *low = gameState->LowEntities + entityIndex;
            
        if(low->HighEntityIndex == 0)
        {
            if(low->Position.AbsTileZ == newCameraPos.AbsTileZ &&
               low->Position.AbsTileX >= minTileX &&
               low->Position.AbsTileX <= maxTileX &&
               low->Position.AbsTileY <= maxTileY &&
               low->Position.AbsTileY >= minTileY)
            {
                MakeEntityHighFrequency(gameState, entityIndex);
            }
        }
    }
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize);      
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {              
        // NOTE(Joey): Reserve entity slot 0 for the null entity
        AddLowEntity(gameState, ENTITY_TYPE_NULL);
        gameState->HighEntityCount = 1; // reserve spot 0 for null entity

        gameState->BackDrop = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_background.bmp");
        gameState->Shadow = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_shadow.bmp");
        
        gameState->HeroBitmaps[0].Head = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_right_head.bmp");
        gameState->HeroBitmaps[0].Torso = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_right_torso.bmp");
        gameState->HeroBitmaps[0].Cape = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_right_cape.bmp");
        gameState->HeroBitmaps[0].AlignX = 72;
        gameState->HeroBitmaps[0].AlignY = 182;
        
        gameState->HeroBitmaps[1].Head = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_back_head.bmp");
        gameState->HeroBitmaps[1].Torso = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_back_torso.bmp");
        gameState->HeroBitmaps[1].Cape = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_back_cape.bmp");
        gameState->HeroBitmaps[1].AlignX = 72;
        gameState->HeroBitmaps[1].AlignY = 182;
        
        gameState->HeroBitmaps[2].Head = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_left_head.bmp");
        gameState->HeroBitmaps[2].Torso = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_left_torso.bmp");
        gameState->HeroBitmaps[2].Cape = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_left_cape.bmp");
        gameState->HeroBitmaps[2].AlignX = 72;
        gameState->HeroBitmaps[2].AlignY = 182;
        
        gameState->HeroBitmaps[3].Head = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_front_head.bmp");
        gameState->HeroBitmaps[3].Torso = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_front_torso.bmp");
        gameState->HeroBitmaps[3].Cape = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_front_cape.bmp");
        gameState->HeroBitmaps[3].AlignX = 72;
        gameState->HeroBitmaps[3].AlignY = 182;
       
        // generate world tile map          
        InitializeArena(&gameState->WorldArena, memory->PermanentStorageSize - sizeof(game_state), (uint8*)memory->PermanentStorage + sizeof(game_state));
        
        gameState->World = PushStruct(&gameState->WorldArena, game_world);
        game_world *world = gameState->World;
        world->TileMap = PushStruct(&gameState->WorldArena, tile_map);
                
        tile_map *tileMap = world->TileMap;
        InitializeTileMap(tileMap, 1.4f);
        
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
        for(uint32 screenIndex = 0; screenIndex < 2; ++screenIndex)
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
                                        
                    SetTileValue(&gameState->WorldArena, world->TileMap, absTileX, absTileY, absTileZ, tileValue);
                    
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
        tile_map_position newCameraPos = {};
        newCameraPos.AbsTileX = screenBaseX*tilesPerWidth + 17/2;
        newCameraPos.AbsTileY = screenBaseY*tilesPerHeight + 9/2;
        newCameraPos.AbsTileZ = screenBaseZ;
        SetCamera(gameState, newCameraPos);
        

        memory->IsInitialized = true;
    }            
    
  
    
    // handle game input 
    for(int controllerIndex = 0; controllerIndex < ArrayCount(input->Controllers); ++controllerIndex)
    {
        game_controller_input *controller = GetController(input, controllerIndex);
        uint32 lowIndex = gameState->PlayerControllerIndex[controllerIndex];
        if(lowIndex == 0)
        {
            if(controller->Start.EndedDown)
            {
                uint32 entityIndex = AddPlayer(gameState);
                gameState->PlayerControllerIndex[controllerIndex] = entityIndex;                
            }
        }
        else
        {
            game_entity controllingEntity = GetHighEntity(gameState, lowIndex);
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
            
            MovePlayer(gameState, controllingEntity, input->dtPerFrame, dAcceleration);
        }
    }
    
    // move camera based on whichever entity we follow
    tile_map *tileMap = gameState->World->TileMap;
    
    // vector2D entityOffsetPerFrame = {};
    game_entity cameraFollowingEntity = GetHighEntity(gameState, gameState->CameraFollowingEntityIndex);
    if(cameraFollowingEntity.High)
    {
        tile_map_position newCameraP = gameState->CameraPos;
        
        newCameraP.AbsTileZ = cameraFollowingEntity.Low->Position.AbsTileZ;
    
        if(cameraFollowingEntity.High->Position.X > 9.0f*tileMap->TileSideInMeters)
            newCameraP.AbsTileX += 17;
        if(cameraFollowingEntity.High->Position.X < -9.0f*tileMap->TileSideInMeters)
            newCameraP.AbsTileX -= 17;
        if(cameraFollowingEntity.High->Position.Y > 5.0f*tileMap->TileSideInMeters)
            newCameraP.AbsTileY += 9;
        if(cameraFollowingEntity.High->Position.Y < -5.0f*tileMap->TileSideInMeters)
            newCameraP.AbsTileY -= 9;
        
        SetCamera(gameState, newCameraP);
    }   
    
                        
      
        
    // render to screen memory      
    int32  TileSideInPixels= 60;
    real32 MetersToPixels = TileSideInPixels / tileMap->TileSideInMeters;       
        
    real32 LowerLeftX = -TileSideInPixels / 2.0f;
    real32 LowerLeftY = screenBuffer->Height;
    
    vector2D screenCenter = { 0.5f*(real32)screenBuffer->Width, 0.5f*(real32)screenBuffer->Height };

    // DrawRectangle(screenBuffer, 0.0f, 0.0f, (real32)screenBuffer->Width, (real32)screenBuffer->Height, 1.0f, 0.5f, 0.0f);
    DrawBitmap(screenBuffer, &gameState->BackDrop, 0.0f, 0.0f);
    
#if 0
    for(int32 relRow = -10; relRow < 10; ++relRow)
    {
        for(int32 relCol = -20; relCol < 20; ++relCol)
        {
            uint32 col = relCol + gameState->CameraPos.AbsTileX;
            uint32 row = relRow + gameState->CameraPos.AbsTileY;
            uint32 tileID = GetTileValue(tileMap, col, row, gameState->CameraPos.AbsTileZ);
            
            if(tileID > 1)
            {
                real32 gray = tileID == 2 ? 1.0f : 0.5f;
                
                if(tileID > 2)
                {
                    gray = 0.1f;
                }
                
                if(col == gameState->CameraPos.AbsTileX && row == gameState->CameraPos.AbsTileY)
                {
                    gray = 0.25f;
                }
             
                vector2D center = { 
                    screenCenter.X - MetersToPixels*gameState->CameraPos.Offset.X + ((real32)relCol * TileSideInPixels),
                    screenCenter.Y + MetersToPixels*gameState->CameraPos.Offset.Y - ((real32)relRow * TileSideInPixels)
                };
                vector2D halfWidths = { 0.5f*TileSideInPixels, 0.5f*TileSideInPixels };
                vector2D min = center - halfWidths;
                vector2D max = center + halfWidths;
                DrawRectangle(screenBuffer, min, max, 0.75f, gray, 0.75f);
            }
            
        }        
    }
#endif
    
    // entity *player = gameState->Entities;
    for(uint32 entityIndex = 0; entityIndex < gameState->HighEntityCount; ++entityIndex)
    {
        high_entity *highEntity = &gameState->HighEntities[entityIndex];            
        low_entity *lowEntity = gameState->LowEntities + highEntity->LowEntityIndex;             
        
        // jump
        real32 dt = input->dtPerFrame;
        real32 ddZ = -9.81f;
        highEntity->Z +=  0.5f*ddZ*Square(dt) + highEntity->dZ*dt;
        highEntity->dZ = highEntity->dZ + ddZ*dt;
        if(highEntity->Z < 0)
            highEntity->Z = 0.0f;
        real32 alpha = 1.0f - highEntity->Z;
        if(alpha < 0)
            alpha = 0.0f;
        
        real32 playerGroundPointX = screenCenter.X + MetersToPixels*highEntity->Position.X;
        real32 playerGroundPointY = screenCenter.Y - MetersToPixels*highEntity->Position.Y;
        real32 playerLeft = playerGroundPointX - 0.5f*MetersToPixels*lowEntity->Width;
        real32 playerTop = playerGroundPointY - 0.5f*MetersToPixels*lowEntity->Height;
        real32 playerR = 1.0f;
        real32 playerG = 0.5f;
        real32 playerB = 0.0f;       
        real32 Z = -MetersToPixels*highEntity->Z;
        
        if(lowEntity->Type == ENTITY_TYPE_PLAYER)
        {
            // DrawRectangle(screenBuffer, { playerLeft, playerTop }, { playerLeft + MetersToPixels*lowEntity->Width, playerTop + MetersToPixels*lowEntity->Height}, playerR, playerG, playerB);
            uint32 facingDirection = highEntity->FacingDirection;
            DrawBitmap(screenBuffer, &gameState->Shadow, playerGroundPointX, playerGroundPointY, gameState->HeroBitmaps[facingDirection].AlignX, gameState->HeroBitmaps[facingDirection].AlignY, alpha);            

            DrawBitmap(screenBuffer, &gameState->HeroBitmaps[facingDirection].Torso, playerGroundPointX, playerGroundPointY + Z, gameState->HeroBitmaps[facingDirection].AlignX, gameState->HeroBitmaps[facingDirection].AlignY);
            DrawBitmap(screenBuffer, &gameState->HeroBitmaps[facingDirection].Cape, playerGroundPointX, playerGroundPointY + Z, gameState->HeroBitmaps[facingDirection].AlignX, gameState->HeroBitmaps[facingDirection].AlignY);
            DrawBitmap(screenBuffer, &gameState->HeroBitmaps[facingDirection].Head, playerGroundPointX, playerGroundPointY + Z, gameState->HeroBitmaps[facingDirection].AlignX, gameState->HeroBitmaps[facingDirection].AlignY);        
        }
        else
        {
            DrawRectangle(screenBuffer, { playerLeft, playerTop }, { playerLeft + MetersToPixels*lowEntity->Width, playerTop + MetersToPixels*lowEntity->Height}, playerR, playerG, playerB);
        }
                  
    }
    

    // GameRender(screenBuffer, gameState);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, 412);
}
