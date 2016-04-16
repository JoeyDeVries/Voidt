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


extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{   
    // NOTE(Joey): temp. debug output hook; replace with elegant platform debug output tooling.
    GlobalPlatformWriteDebugOutput = memory->PlatformWriteDebugOutput;

    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize);      
    Assert(sizeof(TransientState) <= memory->TransientStorageSize);  
    
    // NOTE(Joey): permanent storage is storage that should never be de-allocated / re-allocated and is
    // required to persist throughout the game's life cycle
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!gameState->IsInitialized)
    {                            
        // set function pointers for Voidt module
        PlatformAddWorkEntry    = memory->PlatformAddWorkEntry;
        PlatformCompleteAllWork = memory->PlatformCompleteAllWork;

        // TODO(Joey): generate procedural world here
        InitializeArena(&gameState->WorldArena, Megabytes(32), (uint8*)memory->PermanentStorage + sizeof(game_state));        
        
        memory_arena mixerArena = {};
        InitializeArena(&mixerArena, Megabytes(1), gameState->WorldArena.Base + gameState->WorldArena.Size);
        gameState->Mixer.MixerArena = mixerArena;              
        
        gameState->IsInitialized = true;
    }            
    // NOTE(Joey): transient memory is memory that could in time be de-allocated / re-allocated (like 
    // game assets when no longer needed anymore.
    TransientState *transientState = (TransientState*)memory->TransientStorage;
    memory_arena *transientArena = &transientState->TransientArena;
    if(!transientState->IsInitialized)
    {
        InitializeArena(&transientState->TransientArena, 
                        memory->TransientStorageSize - sizeof(TransientState), 
                        (uint8*)memory->TransientStorage + sizeof(TransientState));
        
        // allocate game assets
        transientState->Assets.Arena = &transientState->TransientArena;
        transientState->Assets.LoadedTextureCount = 0;
        transientState->Assets.LoadedSoundCount = 0;
        transientState->Assets.DEBUGPlatformReadEntireFile = memory->DEBUGPlatformReadEntireFile;
        transientState->Assets.WorkQueue = memory->WorkQueueLowPriority;
        
        // pre-fetch 
        PreFetchTexture(&transientState->Assets, "space/background.bmp");
        PreFetchTexture(&transientState->Assets, "space/player.bmp");
        PreFetchTexture(&transientState->Assets, "space/enemy.bmp");

        PreFetchSound(&transientState->Assets, "audio/music.wav", true);
        PreFetchSound(&transientState->Assets, "audio/gun.wav");
        PreFetchSound(&transientState->Assets, "audio/explosion.wav");               
        
        PlaySound(&gameState->Mixer, GetSound(&transientState->Assets, "audio/music.wav"), 0.75f);        
         
        transientState->IsInitialized = true;
    }
  
    gameState->FireDelay += input->dtPerFrame;    
    gameState->ExplosionDelay += input->dtPerFrame;    
    //////////////////////////////////////////////////////////
    //       GAME INPUT 
    //////////////////////////////////////////////////////////   
    for(int controllerIndex = 0; controllerIndex < ArrayCount(input->Controllers); ++controllerIndex)
    {
        game_controller_input *controller = GetController(input, controllerIndex);
        // controlled_player *controlledPlayer = gameState->ControlledPlayers + controllerIndex;
        // if(controllerIndex == 0)
        {            
            // movement tuning
            real32 cameraSpeed = 4.0f;
            if(controller->MoveUp.EndedDown)
            {
                // controlledPlayer->Acceleration.y =  1.0f;
                gameState->CameraPos.y += cameraSpeed;
            }
            if(controller->MoveDown.EndedDown)
            {
                // controlledPlayer->Acceleration.y = -1.0f;
                gameState->CameraPos.y -= cameraSpeed;
            }
            if(controller->MoveLeft.EndedDown)
            {
                // controlledPlayer->Acceleration.x = -1.0f;
                gameState->CameraPos.x -= cameraSpeed;
            }
            if(controller->MoveRight.EndedDown)
            {
                // controlledPlayer->Acceleration.x =  1.0f;
                gameState->CameraPos.x += cameraSpeed;
            }

            if(controller->RightShoulder.EndedDown && gameState->FireDelay >= 0.2f)                
            {
                PlaySound(&gameState->Mixer, GetSound(&transientState->Assets, "audio/gun.wav"), 0.55f);
                gameState->FireDelay = 0.0f;
            }
            if (controller->LeftShoulder.EndedDown && gameState->ExplosionDelay >= 1.0f)
            {
                PlaySound(&gameState->Mixer, GetSound(&transientState->Assets, "audio/explosion.wav"), 0.55f);
                gameState->ExplosionDelay = 0.0f;
            }
        }
    }
    
    
    
    //////////////////////////////////////////////////////////
    //       RENDER
    //////////////////////////////////////////////////////////         
    // TODO(Joey): make sure no other allocations happen in transient arena in the meantime
    // like: allocation of assets when requested (make sure this happens in different arena 
    // otherwise).
    TempMemory tempRenderMemory = BeginTempMemory(transientArena);
    
    Texture screenTexture = CreateEmptyTexture(transientArena, screenBuffer->Width, screenBuffer->Height);
    RenderQueue *renderQueue = CreateRenderQueue(transientArena, 256); 
        
    // background
    vector2D screenSize = { (real32)screenBuffer->Width, (real32)screenBuffer->Height };
    vector2D screenCenter = 0.5f*screenSize;
    PushTexture(renderQueue, 
                GetTexture(&transientState->Assets, "space/background.bmp"),
                screenCenter, 
                0, 
                screenSize);

    real32 angle = gameState->TimePassed;
    
    // player
    vector2D cameraPos = gameState->CameraPos;
    vector2D playerPos = { -50.0f, 100.0f };
    vector2D playerRelCamera = playerPos - cameraPos; 
    
    vector2D basisX = Normalize({ (real32)cos(angle), (real32)sin(angle)});
    vector2D basisY = Perpendicular(basisX);
    PushTexture(renderQueue, 
                GetTexture(&transientState->Assets, "space/player.bmp"), 
                screenCenter + playerRelCamera, 
                0,
                { (real32)80, (real32)100 },
                basisX,
                basisY,
                { 1.0f, 1.0f, 1.0f, 0.5f });

    // enemy
    vector2D enemyPos = { 350.0f, -150.0f };
    vector2D enemeyRelCamera = enemyPos - cameraPos;
    angle = 42.30f + gameState->TimePassed * 0.1f;
    basisX = Normalize({ (real32)cos(angle), (real32)sin(angle)});
    basisY = Perpendicular(basisX);
    PushTexture(renderQueue, 
                GetTexture(&transientState->Assets, "space/enemy.bmp"),
                screenCenter + enemeyRelCamera,
                0,
                { 200.0f, 200.0f }, 
                basisX, 
                basisY, 
                { 1.0f, 1.0f, 1.0f, 1.0f });
                
    // render to target
    RenderPass(memory->WorkQueueHighPriority, renderQueue, &screenTexture);

    // output to screen
    BlitTextureToScreen(screenBuffer, &screenTexture);
    
    EndTempMemory(tempRenderMemory);

    // PrintCPUTiming(0);
    // PrintCPUTiming(1);
    
    gameState->TimePassed += input->dtPerFrame;
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    
    // OutputTestSineWave(soundBuffer, 412);
    
    TempMemory mixResultMemory = BeginTempMemory(&gameState->Mixer.MixerArena);
    
    real32 *realChannel0 = PushArray(&gameState->Mixer.MixerArena, soundBuffer->SampleCount, real32);
    real32 *realChannel1 = PushArray(&gameState->Mixer.MixerArena, soundBuffer->SampleCount, real32);    

    MixSounds(&gameState->Mixer, realChannel0, realChannel1, soundBuffer->SampleCount);
    
    real32 *source0 = realChannel0;
    real32 *source1 = realChannel1;
    int16 *sampleOut = soundBuffer->Samples;
    for(int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
    {               
        *sampleOut++ = (int16)(realChannel0[sampleIndex] + 0.5f);
        *sampleOut++ = (int16)(realChannel1[sampleIndex] + 0.5f);
    }        
    
    EndTempMemory(mixResultMemory);
}
