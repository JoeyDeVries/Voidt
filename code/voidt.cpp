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
    // function pointers for threaded work queue
    PlatformAddWorkEntry    = memory->PlatformAddWorkEntry;
    PlatformCompleteAllWork = memory->PlatformCompleteAllWork;

    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize);      
    Assert(sizeof(TransientState) <= memory->TransientStorageSize);  
    
    // NOTE(Joey): permanent storage is storage that should never be de-allocated / re-allocated and is
    // required to persist throughout the game's life cycle
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!gameState->IsInitialized)
    {                                    
        // TODO(Joey): generate procedural world here
        InitializeArena(&gameState->WorldArena, Megabytes(32), (uint8*)memory->PermanentStorage + sizeof(game_state));        
        
        memory_arena mixerArena = {};
        InitializeArena(&mixerArena, Megabytes(1), gameState->WorldArena.Base + gameState->WorldArena.Size);
        gameState->Mixer.MixerArena = mixerArena;        
        InitSoundMixer(&gameState->Mixer);

        GlobalRandom = Seed(1337);
        
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
        
        gameState->Music = PlaySound(&gameState->Mixer, GetSound(&transientState->Assets, "audio/music.wav"), 0.0f, 1.5f, true);   
        SetVolume(gameState->Music, 1.0f, 1.0f, 2.5f);
        // SetVolume(sound, 0.75f, 0.75f, 7.5f);
         
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
                real32 pitch = 1.0f;
                uint32 choice = RandomChoice(&GlobalRandom, 4);
                if(choice == 0) pitch = 1.15f;
                if(choice == 1) pitch = 1.25f;
                if(choice == 2) pitch = 0.85f;
                PlaySound(&gameState->Mixer, GetSound(&transientState->Assets, "audio/gun.wav"), 1.0f, pitch);
                gameState->FireDelay = 0.0f;
            }
            if (controller->LeftShoulder.EndedDown && gameState->ExplosionDelay >= 1.0f)
            {
                PlaySound(&gameState->Mixer, GetSound(&transientState->Assets, "audio/explosion.wav"), 1.0f, 0.8f);
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
    
    // NOTE(Joey): mix in 8-packed 16 bit samples at a time in 128 bit SIMD registers
    Assert((soundBuffer->SampleCount & 7) == 0);
    u32 sampleCount4 = soundBuffer->SampleCount / 4;
    u32 sampleCount8 = soundBuffer->SampleCount / 8;
    __m128 *realChannel0 = PushArray(&gameState->Mixer.MixerArena, sampleCount4, __m128, 16);
    __m128 *realChannel1 = PushArray(&gameState->Mixer.MixerArena, sampleCount4, __m128, 16);    
    
    
    MixSounds(&gameState->Mixer, soundBuffer->SamplesPerSecond, realChannel0, realChannel1, soundBuffer->SampleCount);
    
    
    __m128 *source0 = realChannel0;
    __m128 *source1 = realChannel1;
    __m128i *sampleOut = (__m128i*)soundBuffer->Samples;
    for(u32 sampleIndex = 0; sampleIndex < sampleCount4; ++sampleIndex)
    {               
        __m128 s0 = _mm_load_ps((float*)source0++);
        __m128 s1 = _mm_load_ps((float*)source1++);

        // NOTE(Joey): round floats to 32 bit integers
        __m128i l = _mm_cvtps_epi32(s0);
        __m128i r = _mm_cvtps_epi32(s1);
        // NOTE(Joey): converts both low and high 64 bit to interleaved format:
        // l0r0l1r1 and l2r2l3r3
        __m128i lr0 = _mm_unpacklo_epi32(l, r);
        __m128i lr1 = _mm_unpackhi_epi32(l, r);
        // NOTE(Joey): pack both hi-low interleaved data together as 16 bits (also clamps to 16 bit range)
        __m128i samples = _mm_packs_epi32(lr0, lr1);

        *sampleOut++ = samples;
        // *sampleOut++ = (int16)(source0[sampleIndex] + 0.5f);
        // *sampleOut++ = (int16)(source1[sampleIndex] + 0.5f);
    }        
    
    EndTempMemory(mixResultMemory);
}
