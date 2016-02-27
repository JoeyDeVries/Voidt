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

#include "map.cpp"
#include "entity.cpp"
#include "sim_region.cpp"

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


struct add_low_entity_result
{
    low_entity *Low;
    uint32 LowIndex;
};

internal add_low_entity_result AddLowEntity(game_state *gameState, entity_type type, world_position pos)
{
    Assert(gameState->LowEntityCount < ArrayCount(gameState->LowEntities));
    uint32 index = gameState->LowEntityCount++;
            
    low_entity *lowEntity = gameState->LowEntities + index;
    *lowEntity = {};    
    lowEntity->Sim.Type = type;
    lowEntity->Position = NullPosition();
    
    // place entity in spatial hash-based world
    ChangeEntityLocation(&gameState->WorldArena, gameState->World, index, lowEntity, pos);

    
    add_low_entity_result result;
    result.Low = lowEntity;
    result.LowIndex = index;
    
    return result;
}


internal add_low_entity_result AddWall(game_state *gameState, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    world_position pos = ChunkPositionFromTilePosition(gameState->World, absTileX, absTileY, absTileZ);
    
    add_low_entity_result entity = AddLowEntity(gameState, ENTITY_TYPE_WALL, pos); 
   
    entity.Low->Sim.Height = gameState->World->TileSideInMeters;
    entity.Low->Sim.Width  = gameState->World->TileSideInMeters;
    SetFlag(&entity.Low->Sim, ENTITY_FLAG_COLLIDES);
    
    return entity;
}

internal void InitHitPoints(low_entity *lowEntity, uint32 hitPointCount)
{
    Assert(hitPointCount <= ArrayCount(lowEntity->Sim.HitPoint));
    lowEntity->Sim.HitPointMax = hitPointCount;
    for(uint32 i = 0; i < lowEntity->Sim.HitPointMax; ++i)
    {
        hit_point *hitPoint = lowEntity->Sim.HitPoint + i;  
        hitPoint->Flags = 0;
        hitPoint->FilledAmount = HIT_POINT_SUB_COUNT;
    }
}

internal add_low_entity_result AddSword(game_state *gameState)
{
    add_low_entity_result entity = AddLowEntity(gameState, ENTITY_TYPE_SWORD, NullPosition());
    
    entity.Low->Sim.Height = 0.5f;
    entity.Low->Sim.Width = 1.0f;
    // SetFlag(&entity.Low->Sim, ENTITY_FLAG_NONSPATIAL);
    
    return entity;
}


internal add_low_entity_result AddPlayer(game_state *gameState)
{
    world_position pos = gameState->CameraPos;
    add_low_entity_result entity = AddLowEntity(gameState, ENTITY_TYPE_PLAYER, pos);
    
   
    // entity.Low->Position = gameState->CameraPos;

    entity.Low->Sim.Height = 0.5f;
    entity.Low->Sim.Width = 1.0f;
    SetFlag(&entity.Low->Sim, ENTITY_FLAG_COLLIDES);

    InitHitPoints(entity.Low, 3);
    
    add_low_entity_result sword = AddSword(gameState);
    entity.Low->Sim.Sword.Index = sword.LowIndex;
    
    // AddCollisionRule(gameState, sword.LowIndex, entity.LowIndex, false);
    
    if(gameState->CameraFollowingEntityIndex == 0)
    {
        gameState->CameraFollowingEntityIndex = entity.LowIndex;
    }    
        
    return entity;
}

internal add_low_entity_result AddMonster(game_state *gameState, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    world_position pos = ChunkPositionFromTilePosition(gameState->World, absTileX, absTileY, absTileZ);    
    add_low_entity_result entity = AddLowEntity(gameState, ENTITY_TYPE_MONSTER, pos);
    
    InitHitPoints(entity.Low, 3);
    
    entity.Low->Sim.Height = 0.5f;
    entity.Low->Sim.Width = 1.0f;
    SetFlag(&entity.Low->Sim, ENTITY_FLAG_COLLIDES);
    
    return entity;
}

internal add_low_entity_result AddFamiliar(game_state *gameState, uint32 absTileX, uint32 absTileY, uint32 absTileZ)
{
    world_position pos = ChunkPositionFromTilePosition(gameState->World, absTileX, absTileY, absTileZ);    
    add_low_entity_result entity = AddLowEntity(gameState, ENTITY_TYPE_FAMILIAR, pos);
    
    entity.Low->Sim.Height = 0.5f;
    entity.Low->Sim.Width = 1.0f;
    SetFlag(&entity.Low->Sim, ENTITY_FLAG_COLLIDES);
    
    return entity;
}



inline void PushPiece(entity_visible_piece_group *group, loaded_bitmap *bitmap, vector2D offset, real32 offsetZ, vector2D align, vector2D dim, vector4D color, real32 entityZC = 1.0f)
{
    Assert(group->PieceCount < ArrayCount(group->Pieces));
    
    entity_visible_piece *piece = &group->Pieces[group->PieceCount++];
    piece->Bitmap = bitmap;
    piece->Offset = group->GameState->MetersToPixels * vector2D { offset.X, -offset.Y } - align; // align is in pixels already
    piece->OffsetZ = offsetZ;
    piece->EntityZC = entityZC;
    piece->R = color.R;
    piece->G = color.G;
    piece->B = color.B;
    piece->A = color.A;    
    piece->Dimension = dim;
    
    
}

inline void PushBitmap(entity_visible_piece_group *group, loaded_bitmap *bitmap, vector2D offset, real32 offsetZ, vector2D align, real32 alpha = 1.0f, real32 entityZC = 1.0f)
{
    PushPiece(group, bitmap, offset, offsetZ, align, { 0, 0 }, { 1.0f, 1.0f, 1.0f, alpha }, entityZC);
}

inline void PushRect(entity_visible_piece_group *group, vector2D offset, real32 offsetZ, vector2D dim, vector4D color, real32 entityZC = 1.0f)
{
    PushPiece(group, 0, offset, offsetZ, { 0, 0 }, dim, color, entityZC);    
}

internal void DrawHitPoints(entity_visible_piece_group *pieceGroup, sim_entity *entity)
{
    if(entity->HitPointMax >= 1)
    {
        vector2D healthDim = { 0.2f, 0.2f };
        real32 spacingX = 2.0f*healthDim.X;
        vector2D hitPosition = { -0.5f*(entity->HitPointMax - 1)*spacingX, -0.25f };
        vector2D dHitPosition = { spacingX, 0.0f };
        
        for(uint32 i = 0; i < entity->HitPointMax; ++i)
        {
            hit_point *hitPoint = entity->HitPoint + i;
            vector4D color = { 0.75f, 0.0f, 0.0f, 1.0f };
            if(hitPoint->FilledAmount == 0)
            {
                color.R = 0.25f;
            }
            PushRect(pieceGroup, hitPosition, 0, healthDim, color, 0.0f);
            hitPosition += dHitPosition;
            
        }
    }    
}


internal void ClearCollisionRules(game_state *gameState, uint32 storageIndex)
{
    // TODO(Joey): make better data structure that allows for removal of collision rules
    // without searching the entire hash table.
    for(uint32 hashBucket = 0; hashBucket < ArrayCount(gameState->CollisionRuleHash); ++hashBucket)
    {
        for(pairwise_collision_rule **rule = &gameState->CollisionRuleHash[hashBucket];
        *rule;)
        {
            if((*rule)->StorageIndexA == storageIndex || (*rule)->StorageIndexB == storageIndex)
            {
                pairwise_collision_rule *removedRule = *rule;
                *rule = (*rule)->NextInHash;
                
                removedRule->NextInHash = gameState->FirstFreeCollisionRule;
                gameState->FirstFreeCollisionRule = removedRule;  
            }
            else
            {
                rule = &(*rule)->NextInHash;
            }
        }
    }
}

internal void AddCollisionRule(game_state *gameState, uint32 storageIndexA, uint32 storageIndexB, bool32 shouldCollide)
{
    if(storageIndexA > storageIndexB) 
    {
        uint32 temp = storageIndexA;
        storageIndexA = storageIndexB;
        storageIndexB = temp;
    }
    
    pairwise_collision_rule *found = 0;
    int32 hashValue = storageIndexA & (ArrayCount(gameState->CollisionRuleHash) - 1);    
    for(pairwise_collision_rule *rule = gameState->CollisionRuleHash[hashValue];
        rule;
        rule = rule->NextInHash)
    {
        if(rule->StorageIndexA == storageIndexA && rule->StorageIndexB == storageIndexB)
        {
            found = rule;
            // result = rule->ShouldCollide;
            break;
        }
    }
    if(!found)
    {   // NOTE(Joey): we didn't find any collision rule, create new one (or take from free list)
        found = gameState->FirstFreeCollisionRule;
        if(found)
        {
            gameState->FirstFreeCollisionRule = found->NextInHash;
        }
        else
        {
            found = PushStruct(&gameState->WorldArena, pairwise_collision_rule);                       
        }
        // insert at the head so point to previous head
        found->NextInHash = gameState->CollisionRuleHash[hashValue];
        gameState->CollisionRuleHash[hashValue] = found;
        
    }
    
    if(found)
    {
        found->StorageIndexA = storageIndexA;
        found->StorageIndexB = storageIndexB;
        found->ShouldCollide = shouldCollide;
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
        AddLowEntity(gameState, ENTITY_TYPE_NULL, NullPosition());

        gameState->BackDrop = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_background.bmp");
        gameState->Shadow = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_hero_shadow.bmp");
        gameState->Tree = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test2/tree00.bmp");
        gameState->Sword = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test2/rock03.bmp");
        
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

        InitializeWorld(world, 1.4f);
        int32  TileSideInPixels = 60;
        gameState->MetersToPixels = TileSideInPixels / world->TileSideInMeters;
        
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

                    if(tileValue == 2)
                        AddWall(gameState, absTileX, absTileY, absTileZ);
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
        gameState->CameraPos = newCameraPos;
        
        AddMonster(gameState, cameraTileX + 2, cameraTileY + 2, cameraTileZ);
        AddFamiliar(gameState, cameraTileX - 2, cameraTileY + 2, cameraTileZ);
        
        
        memory->IsInitialized = true;
    }            
    
  
    
    //////////////////////////////////////////////////////////
    //       GAME INPUT 
    //////////////////////////////////////////////////////////   
    for(int controllerIndex = 0; controllerIndex < ArrayCount(input->Controllers); ++controllerIndex)
    {
        game_controller_input *controller = GetController(input, controllerIndex);
        controlled_player *controlledPlayer = gameState->ControlledPlayers + controllerIndex;
        if(controlledPlayer->EntityIndex == 0)
        {
            if(controller->Start.EndedDown)
            {
                *controlledPlayer = {};
                controlledPlayer->EntityIndex = AddPlayer(gameState).LowIndex;
                // gameState->PlayerControllerIndex[controllerIndex] = entity.LowIndex;                
            }
        }
        else
        {
            controlledPlayer->Acceleration = {};
            controlledPlayer->dZ = 0.0f;
            controlledPlayer->AccelerationSword = {};
            if(controller->IsAnalog)
            {
                // analog movement tuning
                controlledPlayer->Acceleration = { controller->StickAverageX, controller->StickAverageY };
            }
            else
            {
                // digital movement tuning
                if(controller->MoveUp.EndedDown)
                {
                    controlledPlayer->Acceleration.Y =  1.0f;
                }
                if(controller->MoveDown.EndedDown)
                {
                    controlledPlayer->Acceleration.Y = -1.0f;
                }
                if(controller->MoveLeft.EndedDown)
                {
                    controlledPlayer->Acceleration.X = -1.0f;
                }
                if(controller->MoveRight.EndedDown)
                {
                    controlledPlayer->Acceleration.X =  1.0f;
                }                                
            }
            
            if(controller->Start.EndedDown)
                controlledPlayer->dZ = 3.0f;
            
           
            if(controller->ActionUp.EndedDown)
                controlledPlayer->AccelerationSword = {  0.0f,  1.0f };    
            if(controller->ActionDown.EndedDown)
                controlledPlayer->AccelerationSword = {  0.0f, -1.0f };    
            if(controller->ActionLeft.EndedDown)
                controlledPlayer->AccelerationSword = { -1.0f,  0.0f };    
            if(controller->ActionRight.EndedDown)
                controlledPlayer->AccelerationSword = {  1.0f,  0.0f };    
        }
    }
    
    
    
    //////////////////////////////////////////////////////////
    //       UPDATE CAMERA
    //////////////////////////////////////////////////////////      
    game_world *world = gameState->World;
    
    uint32 tileSpanX = 17*3;
    uint32 tileSpanY =  9*3;
    rectangle2D cameraBounds = RectCenterDim(vector2D { 0, 0 }, world->TileSideInMeters*vector2D { (real32)tileSpanX, (real32)tileSpanY });                 
      
      
      
    memory_arena simArena = {};
    InitializeArena(&simArena, memory->TransientStorageSize,  memory->TransientStorage);
    sim_region *simRegion = BeginSimulation(&simArena, gameState, world, gameState->CameraPos, cameraBounds);
    
    
    
                        
      
        
    // render to screen memory       
    real32 MetersToPixels = gameState->MetersToPixels;       
         
    vector2D screenCenter = { 0.5f*(real32)screenBuffer->Width, 0.5f*(real32)screenBuffer->Height };

    DrawRectangle(screenBuffer, { 0.0f, 0.0f }, {(real32)screenBuffer->Width, (real32)screenBuffer->Height }, 0.5f, 0.5f, 0.5f);
    // DrawBitmap(screenBuffer, &gameState->BackDrop, 0.0f, 0.0f);
    

        
    //////////////////////////////////////////////////////////
    //       RENDER 
    //////////////////////////////////////////////////////////        
    entity_visible_piece_group pieceGroup;
    pieceGroup.GameState = gameState;
    sim_entity *entity = simRegion->Entities;
    for(uint32 entityIndex = 0; entityIndex < simRegion->EntityCount; ++entityIndex, ++entity)
    {
        if(entity->Updatable)
        {
            pieceGroup.PieceCount = 0;         
            
            real32 dt = input->dtPerFrame;
                     
             
            real32 alpha = 1.0f - entity->Z;
            if(alpha < 0)
                alpha = 0.0f;                           
            
            move_spec moveSpec = DefaultMoveSpec();
            vector2D acceleration = {};
            
            hero_bitmaps *heroBitmaps = &gameState->HeroBitmaps[entity->FacingDirection];
            
            switch(entity->Type)
            {
                case ENTITY_TYPE_PLAYER:
                {
                    // NOTE(Joey): figure out which controller is running this player and get its input requests
                    for(uint32 controllerIndex = 0; controllerIndex < ArrayCount(gameState->ControlledPlayers); ++controllerIndex)
                    {
                        controlled_player *controlledPlayer = gameState->ControlledPlayers + controllerIndex;
                        if(entity->StorageIndex == controlledPlayer->EntityIndex)
                        {
                            if(controlledPlayer->dZ != 0.0f)
                                entity->dZ = controlledPlayer->dZ;
                            
                            // move_spec moveSpec = DefaultMoveSpec();
                            moveSpec.UnitMaxAccelVector = true;
                            moveSpec.Speed = 50.0f;
                            moveSpec.Drag = 8.0f;
                            acceleration = controlledPlayer->Acceleration;    
                            
                            
                            if(controlledPlayer->AccelerationSword.X != 0.0f || controlledPlayer->AccelerationSword.Y != 0.0f)
                            {   // initiate sword sequence if not yet active
                                sim_entity *sword = entity->Sword.Ptr;
                                if(sword && IsSet(sword, ENTITY_FLAG_NONSPATIAL))
                                {
                                    sword->DistanceLimit = 5.0f;
                                    MakeEntitySpatial(sword, entity->Position,  entity->Velocity + 5.0f*controlledPlayer->AccelerationSword);
                                    AddCollisionRule(gameState, sword->StorageIndex, entity->StorageIndex, false);
                                }
                            }
                    
                            
                        }
                    }            
                    
                   
                    
                    // DrawRectangle(screenBuffer, { playerLeft, playerTop }, { playerLeft + MetersToPixels*lowEntity->Sim.Width, playerTop + MetersToPixels*lowEntity->Sim.Height}, playerR, playerG, playerB);
                    
                    PushBitmap(&pieceGroup, &gameState->Shadow, { 0, 0 }, 0, heroBitmaps->Align, alpha, 0.0f);            
                    PushBitmap(&pieceGroup, &heroBitmaps->Torso, { 0, 0 }, 0, heroBitmaps->Align);
                    PushBitmap(&pieceGroup, &heroBitmaps->Cape, { 0, 0}, 0, heroBitmaps->Align);
                    PushBitmap(&pieceGroup, &heroBitmaps->Head, { 0, 0 }, 0, heroBitmaps->Align);     
                    
                    DrawHitPoints(&pieceGroup, entity);
                   
                } break;        
                case ENTITY_TYPE_WALL:
                {
                    PushBitmap(&pieceGroup, &gameState->Tree, { 0, 0 }, 0, { 40, 80 });
                    // DrawRectangle(screenBuffer, { playerLeft, playerTop }, { playerLeft + MetersToPixels*lowEntity->Sim.Width, playerTop + MetersToPixels*lowEntity->Sim.Height}, playerR, playerG, playerB);
                } break;     
                case ENTITY_TYPE_SWORD:            
                {
                    moveSpec.UnitMaxAccelVector = false;
                    moveSpec.Speed = 0.0f;
                    moveSpec.Drag = 0.0f;

                    vector2D oldPos = entity->Position;
                    // real32 distanceTraveled = Length(entity->Position - oldPos);

                    // entity->DistanceRemaining -= distanceTraveled;
                    if(entity->DistanceLimit == 0.0f)
                    {
                        ClearCollisionRules(gameState, entity->StorageIndex);
                        MakeEntityNonSpatial(entity);
                    }
                    
                    PushBitmap(&pieceGroup, &gameState->Shadow, { 0, 0 }, 0, heroBitmaps->Align, alpha);            
                    PushBitmap(&pieceGroup, &gameState->Sword, { 0, 0 }, 0, { 29, 10 });                
                } break;
                case ENTITY_TYPE_MONSTER:
                {
                    UpdateMonster(simRegion, entity, dt);
                    PushBitmap(&pieceGroup, &gameState->Shadow, { 0, 0 }, 0, heroBitmaps->Align, alpha);            
                    PushBitmap(&pieceGroup, &heroBitmaps->Torso, { 0, 0 }, 0, heroBitmaps->Align);  
                    
                    DrawHitPoints(&pieceGroup, entity);
                } break;
                case ENTITY_TYPE_FAMILIAR:
                {
                    sim_entity *closestPlayer = 0;
                    real32 closestPlayerDSq = Square(10.0f); // NOTE(Joey): don't search for more than 10 meters
                    sim_entity *testEntity = simRegion->Entities;
                    for(uint32 testEntityIndex = 0; testEntityIndex < simRegion->EntityCount; ++testEntityIndex, ++testEntity)
                    {
                        if(testEntity->Type == ENTITY_TYPE_PLAYER)
                        {
                            real32 testDSq = LengthSq(testEntity->Position - entity->Position);
                            if(testDSq <= closestPlayerDSq)
                            {
                                closestPlayer = testEntity;
                                closestPlayerDSq = testDSq;
                            }
                        }
                    }
                    
                    if(closestPlayer && closestPlayerDSq > Square(3.0f))
                    {
                        real32 speed = 0.5f;
                        real32 oneOverLength = speed / SquareRoot(closestPlayerDSq);
                        acceleration = oneOverLength * (closestPlayer->Position - entity->Position);
                        
                    }

                    moveSpec.UnitMaxAccelVector = true;
                    moveSpec.Speed = 50.0f;
                    moveSpec.Drag = 8.0f;
              
                    PushBitmap(&pieceGroup, &gameState->Shadow, { 0, 0 }, 0, heroBitmaps->Align, alpha, 0.0f);            
                    PushBitmap(&pieceGroup, &heroBitmaps->Head, { 0, 0 }, 0, heroBitmaps->Align);        
                } break;
                default:
                    InvalidCodePath;
                    break;
            }            
            
            if(!IsSet(entity, ENTITY_FLAG_NONSPATIAL))
            {             
                MoveEntity(gameState, simRegion, entity, input->dtPerFrame, &moveSpec, acceleration);
            }
            
            real32 entityGroundPointX = screenCenter.X + MetersToPixels*entity->Position.X;
            real32 entityGroundPointY = screenCenter.Y - MetersToPixels*entity->Position.Y;
            real32 Z = -MetersToPixels*entity->Z; 
            
            for(uint32 pieceIndex = 0; pieceIndex < pieceGroup.PieceCount; ++pieceIndex)
            {
                entity_visible_piece *piece = &pieceGroup.Pieces[pieceIndex];
                vector2D center = { entityGroundPointX + piece->Offset.X, entityGroundPointY + piece->Offset.Y + piece->OffsetZ + piece->EntityZC*Z };
                if(piece->Bitmap)
                {
                    DrawBitmap(screenBuffer, piece->Bitmap, center.X, center.Y, piece->A);
                }
                else
                {   
                    vector2D halfDim = 0.5f*MetersToPixels*piece->Dimension;
                    DrawRectangle(screenBuffer, center - halfDim, center + halfDim, piece->R, piece->G, piece->B);
                }
            }
        }
    }
    
    EndSimulation(simRegion, gameState);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, 412);
}
