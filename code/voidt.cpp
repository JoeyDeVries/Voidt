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

internal void DrawBitmap(game_offscreen_buffer *buffer, loaded_bitmap *bitmap, real32 X, real32 Y, int32 alignX = 0, int32 alignY = 0)
{
    X -= (real32)alignX;
    Y -= (real32)alignY;
    
    int32 minX = RoundReal32ToInt32(X);
    int32 minY = RoundReal32ToInt32(Y);
    int32 maxX = RoundReal32ToInt32(X + bitmap->Width);
    int32 maxY = RoundReal32ToInt32(Y + bitmap->Height);
    
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
        
        uint32 *sourceDest = pixels;
        for(int32 y = 0; y < result.Height; ++y)
        {
            for(int x = 0; x < result.Width; ++x)
            {
                uint32 C = *sourceDest;
                *sourceDest++ = (((C >> alphaShift) & 0xFF) << 24) | 
                                (((C >> redShift  ) & 0xFF) << 16) | 
                                (((C >> greenShift) & 0xFF) << 8)  | 
                                (((C >> blueShift ) & 0xFF) << 0); 
            }            
        }
    }
    return result;
}

inline entity* GetEntity(game_state *state, uint32 index)
{
    entity *result = 0;
    if(index > 0 && index < ArrayCount(state->Entities))
    {
        result = &state->Entities[index];
    }
    return result;
}

internal uint32 AddEntity(game_state *state)
{
    uint32 index = state->EntityCount++;
    
    Assert(state->EntityCount < ArrayCount(state->Entities));
    entity *ent = &state->Entities[index];
    *ent = {};
    
    return index;
}

internal void InitializePlayer(game_state *state, uint32 entityIndex)
{
    entity *player = GetEntity(state, entityIndex);
    
    player->Exists = true;
    
    player->Position.AbsTileX = 1;
    player->Position.AbsTileY = 3;
    player->Position.AbsTileZ = 0;
    player->Position.Offset.X = 5.0f;
    player->Position.Offset.X = 5.0f;
    
    player->Height = 1.4f;
    player->Width = 0.75f*player->Height;
    
    if(!GetEntity(state, state->CameraFollowingEntityIndex))
    {
        state->CameraFollowingEntityIndex = entityIndex;
    }    
}



internal void MovePlayer(game_state *state, entity *player, real32 dt, vector2D acceleration)
{
    tile_map *tileMap = state->World->TileMap;
    
    real32 speed = 50.0f;
    acceleration *= speed;
            
    // add friction (approximation) to acceleration
    acceleration += -8.0f*player->Velocity;
            
    // update player position (use velocity at begin of frame)
    tile_map_position oldPlayerPos = player->Position;
    tile_map_position newPlayerPos = player->Position;
    vector2D playerDelta = 0.5f * acceleration*Square(dt) + player->Velocity*dt;
    newPlayerPos.Offset += playerDelta;
    newPlayerPos = CorrectTileMapPosition(tileMap, newPlayerPos);
            
    // update velocity (add acceleration to velocity as final velocity end of frame)
    player->Velocity += acceleration * dt;
            
    // detect collisions
    tile_map_position playerTop = newPlayerPos;
    playerTop.Offset.Y += 0.25;
    playerTop = CorrectTileMapPosition(tileMap, playerTop);
    
    tile_map_position playerLeft = newPlayerPos;
    playerLeft.Offset.X -= 0.5f*player->Width;
    playerLeft = CorrectTileMapPosition(tileMap, playerLeft);
    
    tile_map_position playerRight = newPlayerPos;
    playerRight.Offset.X += 0.5f*player->Width;
    playerRight = CorrectTileMapPosition(tileMap, playerRight);
                        
    bool32 collided = false;
    tile_map_position colPos = {};
    if(!IsTileMapPointEmpty(tileMap, playerTop))
    {
        colPos = playerTop;
        collided = true;
    }
    if(!IsTileMapPointEmpty(tileMap, playerLeft))
    {
        colPos = playerLeft;
        collided = true;
    }
    if(!IsTileMapPointEmpty(tileMap, playerRight))
    {
        colPos = playerRight;
        collided = true;
    }
            
    if(collided)
    {
       vector2D reflection = { 0.0f, 0.0f };
        
        if(colPos.AbsTileX < player->Position.AbsTileX)
            reflection = vector2D {  1.0f, 0.0f };
        if(colPos.AbsTileX > player->Position.AbsTileX)
            reflection = vector2D { -1.0f, 0.0f };
        if(colPos.AbsTileY < player->Position.AbsTileY)  
            reflection = vector2D { 0.0f, 1.0f }; 
        if(colPos.AbsTileY > player->Position.AbsTileY)
            reflection = vector2D { 0.0, -1.0f };
        
        player->Velocity = player->Velocity - 2*InnerProduct(player->Velocity, reflection)*reflection;
    }
    else
    {          
        player->Position = newPlayerPos;                             
    }           
                    
    if(!AreOnSameTile(&oldPlayerPos, &player->Position))
    {
        uint32 tileValue = GetTileValue(tileMap, player->Position);
        if(tileValue == 3)
            ++player->Position.AbsTileZ;
        if(tileValue == 4)
            --player->Position.AbsTileZ;
    }     
    
    // determine new facing direction
    if(player->Velocity.X == 0.0f && player->Velocity.Y == 0.0f)
    {
        // NOTE(Joey): leave facing direction to earlier set facing direction
    }
    else if(Absolute(player->Velocity.X) > Absolute(player->Velocity.Y))
    {
        player->FacingDirection = player->Velocity.X > 0 ? 0 : 2;            
    }
    else
    {
        player->FacingDirection = player->Velocity.Y > 0 ? 1 : 3;
    }               
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize); 
    
    real32 playerWidth = 0.75f*1.4f;
    real32 playerHeight = 1.4f;
    
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {              
        // NOTE(Joey): Reserve entity slot 0 for the null entity
        AddEntity(gameState);

        gameState->BackDrop = DEBUGLoadBMP(thread, memory->DEBUGPlatformReadEntireFile, "test/test_background.bmp");
        
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

        gameState->CameraPos.AbsTileX = 17/2;
        gameState->CameraPos.AbsTileY = 9/2;
        
        // generate world tile map          
        InitializeArena(&gameState->WorldArena, memory->PermanentStorageSize - sizeof(game_state), (uint8*)memory->PermanentStorage + sizeof(game_state));
        
        gameState->World = PushStruct(&gameState->WorldArena, game_world);
        game_world *world = gameState->World;
        world->TileMap = PushStruct(&gameState->WorldArena, tile_map);
                
        tile_map *tileMap = world->TileMap;
        
        tileMap->ChunkShift = 4;
        tileMap->ChunkMask = (1 << tileMap->ChunkShift) - 1;
        tileMap->ChunkDim = (1 << tileMap->ChunkShift);
        
        tileMap->TileChunkCountX = 128;
        tileMap->TileChunkCountY = 128;
        tileMap->TileChunkCountZ = 2;
        tileMap->TileChunks = PushArray(&gameState->WorldArena, tileMap->TileChunkCountX*tileMap->TileChunkCountY*tileMap->TileChunkCountZ, tile_chunk);       
        
        tileMap->TileSideInMeters = 1.4f;

        uint32 tilesPerWidth = 17;
        uint32 tilesPerHeight = 9;
        
        uint32 screenX = 0;
        uint32 screenY = 0;
        uint32 absTileZ = 0;
        bool doorTop = false;
        bool doorBottom = false;
        bool doorLeft = false;
        bool doorRight = false;
        bool doorUp  = false;
        bool doorDown = false;
        for(uint32 screenIndex = 0; screenIndex < 100; ++screenIndex)
        {
            uint32 randomChoice = 0;
            if(doorUp || doorDown)
                randomChoice = (885447 * screenIndex + (screenIndex + 7) * 7637 / 937) % 2;
            else
                randomChoice = (885447 * screenIndex + (screenIndex + 7) * 7637 / 937) % 3;

            bool createdZDoor = false;
            if(randomChoice == 2)
            {
                createdZDoor = true;
                if (absTileZ == 0)
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
                if(absTileZ == 0) 
                {
                    absTileZ = 1;
                }
                else 
                {
                    absTileZ = 0;
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

        memory->IsInitialized = true;
    }            
    
  
    
    // handle game input 
    for(int controllerIndex = 0; controllerIndex < ArrayCount(input->Controllers); ++controllerIndex)
    {
        game_controller_input *controller = GetController(input, controllerIndex);
        entity *controllingEntity = GetEntity(gameState, gameState->PlayerControllerIndex[controllerIndex]);
        if(controllingEntity)
        {
            vector2D dAcceleration = {};
            if(controller->IsAnalog)
            {
                // analog movement tuning
                dAcceleration = vector2D { controller->StickAverageX, controller->StickAverageY };
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
            MovePlayer(gameState, controllingEntity, input->dtPerFrame, dAcceleration);
        }
        else
        {
            if(controller->Start.EndedDown)
            {
                uint32 entityIndex = AddEntity(gameState);
                InitializePlayer(gameState, entityIndex);
                gameState->PlayerControllerIndex[controllerIndex] = entityIndex;                
            }
        }
    }
    
    // move camera based on whichever entity we follow
    tile_map *tileMap = gameState->World->TileMap;
    entity *cameraFollowingEntity = GetEntity(gameState, gameState->CameraFollowingEntityIndex);
    if(cameraFollowingEntity)
    {
        gameState->CameraPos.AbsTileZ = cameraFollowingEntity->Position.AbsTileZ;
    
         tile_map_difference diff = Subtract(gameState->World->TileMap, &cameraFollowingEntity->Position, &gameState->CameraPos);
        if(diff.dX > 9.0f*tileMap->TileSideInMeters)
            gameState->CameraPos.AbsTileX += 17;
        if(diff.dX < -9.0f*tileMap->TileSideInMeters)
            gameState->CameraPos.AbsTileX -= 17;
        if(diff.dY > 5.0f*tileMap->TileSideInMeters)
            gameState->CameraPos.AbsTileY += 9;
        if(diff.dY < -5.0f*tileMap->TileSideInMeters)
            gameState->CameraPos.AbsTileY -= 9;
    }   
    
                        
            //
            // new collision code
            //
            // uint32 minTileX = 0;
            // uint32 minTileY = 0;
            // uint32 onePastMaxTileX = 0;
            // uint32 onePastMaxTileY = 0;
            // uint32 absTileZ = gameState->PlayerPos.AbsTileZ;
            // tile_map_position bestPlayerPos = gameState->PlayerPos;
            // real32 bestDistanceSq = LengthSq(playerDelta);
            
            // for(uint32 absTileY = minTileY; absTileY != onePastMaxTileY; ++absTileY)
            // {
                // for(uint32 absTileX = minTileX; absTileX != onePastMaxTileX; ++absTileX)
                // {
                    // tile_map_position testTilePos = CenteredTilePoint(absTileX, absTileY, absTileZ);
                    // uint32 tileValue = GetTileValue(tileMap, testTilePos);
                    
                    // if(IsTileValueEmpty(tileValue))
                    // {
                        // vector2D minCorner = -0.5f * vector2D { tileMap->TileSideInMeters, tileMap->TileSideInMeters };
                        // vector2D maxCorner =  0.5f * vector2D { tileMap->TileSideInMeters, tileMap->TileSideInMeters };
                        
                        // tile_map_difference relNewPlayerPos = Subtract(tileMap, &testTilePos, &newPlayerPos);
                        // vector2D testPos = ClosestPointInRectangle(minCorner, maxCorner, relNewPlayerPos);
                        
                        // real32 testDistanceSq = ;
                        // if(bestDistanceSq > testDistanceSq)
                        // {
                            // bestPlayerPos = ;
                            // bestDistanceSq = ;
                        // }
                        
                    // }
                // }
            // }
                          
        
    // render to screen memory      
    int32  TileSideInPixels= 60;
    real32 MetersToPixels = TileSideInPixels / tileMap->TileSideInMeters;       
        
    real32 LowerLeftX = -TileSideInPixels / 2.0f;
    real32 LowerLeftY = screenBuffer->Height;
    
    vector2D screenCenter = { 0.5f*(real32)screenBuffer->Width, 0.5f*(real32)screenBuffer->Height };

    // DrawRectangle(screenBuffer, 0.0f, 0.0f, (real32)screenBuffer->Width, (real32)screenBuffer->Height, 1.0f, 0.5f, 0.0f);
    DrawBitmap(screenBuffer, &gameState->BackDrop, 0.0f, 0.0f);
    
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
    
    entity *player = gameState->Entities;
    for(uint32 i = 0; i < gameState->EntityCount; ++i, ++player)
    {
        if(player->Exists)
        {
            tile_map_difference diff = Subtract(gameState->World->TileMap, &player->Position, &gameState->CameraPos);
    
            real32 playerGroundPointX = screenCenter.X + MetersToPixels*diff.dX;
            real32 playerGroundPointY = screenCenter.Y - MetersToPixels*diff.dY;
            real32 playerLeft = playerGroundPointX - 0.5f*MetersToPixels*playerWidth;
            real32 playerTop = playerGroundPointY - MetersToPixels*playerHeight;
            real32 playerR = 1.0f;
            real32 playerG = 0.5f;
            real32 playerB = 0.0f;       
            DrawRectangle(screenBuffer, { playerLeft, playerTop }, { playerLeft + MetersToPixels*playerWidth, playerTop + MetersToPixels*playerHeight }, playerR, playerG, playerB);
            uint32 facingDirection = player->FacingDirection;
            DrawBitmap(screenBuffer, &gameState->HeroBitmaps[facingDirection].Torso, playerGroundPointX, playerGroundPointY, gameState->HeroBitmaps[facingDirection].AlignX, gameState->HeroBitmaps[facingDirection].AlignY);
            DrawBitmap(screenBuffer, &gameState->HeroBitmaps[facingDirection].Cape, playerGroundPointX, playerGroundPointY, gameState->HeroBitmaps[facingDirection].AlignX, gameState->HeroBitmaps[facingDirection].AlignY);
            DrawBitmap(screenBuffer, &gameState->HeroBitmaps[facingDirection].Head, playerGroundPointX, playerGroundPointY, gameState->HeroBitmaps[facingDirection].AlignX, gameState->HeroBitmaps[facingDirection].AlignY);            
        }        
    }
    

    // GameRender(screenBuffer, gameState);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, 412);
}
