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

// #include "map.cpp"
// #include "entity.cpp"
// #include "sim_region.cpp"

// #include "renderer/renderer.cpp"

internal void DrawRectangle(game_offscreen_buffer *buffer, vector2D min, vector2D max, real32 r, real32 g, real32 b)
{  
    int32 minX = RoundReal32ToInt32(min.x);
    int32 minY = RoundReal32ToInt32(min.y);
    int32 maxX = RoundReal32ToInt32(max.x);
    int32 maxY = RoundReal32ToInt32(max.y);
    
    if(minX < 0) minX = 0;
    if(minY < 0) minY = 0;
    if(maxX > buffer->Width) maxX = buffer->Width;
    if(maxY > buffer->Height) maxY = buffer->Height;
    
    // BIT PATTERN: 0x AA RR GG BB
    uint32 color = (RoundReal32ToUInt32(r * 255.0f) << 16) |
                   (RoundReal32ToUInt32(g * 255.0f) << 8)  |
                   (RoundReal32ToUInt32(b * 255.0f) << 0);
        
    uint8* row = (uint8*)buffer->Memory + minX * sizeof(uint32) + minY * buffer->Pitch;
    
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

/*
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
*/

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

/*
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
        
        Assert(result.Height > 0); // we have a bottom-up bitmap; otherwise assert as we don't handle top-down bitmaps
        
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
    result.Pitch = result.Width*sizeof(uint32);
    return result;
}
*/

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize);      
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {                    
        gameState->Background = LoadTexture(thread, memory->DEBUGPlatformReadEntireFile, "space/background.bmp");
        gameState->Player = LoadTexture(thread, memory->DEBUGPlatformReadEntireFile, "space/player.bmp");
        gameState->Enemy = LoadTexture(thread, memory->DEBUGPlatformReadEntireFile, "space/enemy.bmp");
       
        //////////////////////////////////////////////////////////
        //       WORLD GENERATION 
        //////////////////////////////////////////////////////////            
        InitializeArena(&gameState->WorldArena, memory->PermanentStorageSize - sizeof(game_state), (uint8*)memory->PermanentStorage + sizeof(game_state));
        
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
                      
            }
        }
        else
        {
            controlledPlayer->Acceleration = {};
            controlledPlayer->Velocity.z = 0.0f;
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
                    controlledPlayer->Acceleration.y =  1.0f;
                }
                if(controller->MoveDown.EndedDown)
                {
                    controlledPlayer->Acceleration.y = -1.0f;
                }
                if(controller->MoveLeft.EndedDown)
                {
                    controlledPlayer->Acceleration.x = -1.0f;
                }
                if(controller->MoveRight.EndedDown)
                {
                    controlledPlayer->Acceleration.x =  1.0f;
                }                                
            }
            
            if(controller->Start.EndedDown)
                controlledPlayer->Velocity.z = 3.0f;
            
           
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
    DrawRectangle(screenBuffer, { 0.0f, 0.0f }, {(real32)screenBuffer->Width, (real32)screenBuffer->Height }, 0.8f, 0.5f, 0.5f);
    
    // NOTE(Joey): render testing bed
    RenderRectangle_(screenBuffer, { 50.0f, 50.0f }, { 100.0f, 100.0f }, { 1.0f, 1.0f, 0.0f, 1.0f });
    
    // background
    vector2D screenSize = { (real32)screenBuffer->Width, (real32)screenBuffer->Height };
    vector2D screenCenter = 0.5f*screenSize;
    RenderTexture(screenBuffer, &gameState->Background, screenCenter, screenSize);

    real32 angle = gameState->TimePassed;
    // player
    vector2D basisX = Normalize({ (real32)cos(angle), (real32)sin(angle)});
    vector2D basisY = Perpendicular(basisX);
    RenderTexture_(screenBuffer, &gameState->Player, { 150.0f, 450.0f }, { (real32)gameState->Player.Width, (real32)gameState->Player.Height }, basisX, basisY, { 1.0f, 1.0f, 1.0f, 1.0f });
    
    // enemey
    angle = 42.30f + gameState->TimePassed * 0.1f;
    basisX = Normalize({ (real32)cos(angle), (real32)sin(angle)});
    basisY = Perpendicular(basisX);
    RenderTexture_(screenBuffer, &gameState->Enemy, { 750.0f, 150.0f }, { 200.0f, 200.0f }, basisX, basisY, { 1.0f, 1.0f, 1.0f, 1.0f });
        
    //////////////////////////////////////////////////////////
    //       RENDER 
    //////////////////////////////////////////////////////////        
    
    gameState->TimePassed += input->dtPerFrame;
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, 412);
}
