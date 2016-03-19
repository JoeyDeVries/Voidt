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
            {
                controlledPlayer->Velocity.z = 3.0f;                
            }
            
           
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
