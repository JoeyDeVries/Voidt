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
    uint16 toneVolume = 3000;
    int wavePeriod = soundBuffer->SamplesPerSecond / toneHz;
    
    int16 *sampleOut = soundBuffer->Samples;
    for(int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
    {
        real32 sineValue = sinf(gameState->tSine);
        int16 sampleValue = (int16)(sineValue * toneVolume);
        // sampleValue = 0; // disable
        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;
        
        gameState->tSine += (2.0f * Pi32) / (real32)wavePeriod;
        
        if(gameState->tSine >= 2.0f * Pi32)
            gameState->tSine -= 2.0f * Pi32;
    }        
} 

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{   
    // NOTE(Joey): temp. debug output hook; replace with elegant platform debug output tooling.
    GlobalPlatformWriteDebugOutput = memory->PlatformWriteDebugOutput;

    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize);      
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {                    
        gameState->Background = LoadTexture(thread, memory->DEBUGPlatformReadEntireFile, "space/background.bmp");
        gameState->Player = LoadTexture(thread, memory->DEBUGPlatformReadEntireFile, "space/player.bmp");
        gameState->Enemy = LoadTexture(thread, memory->DEBUGPlatformReadEntireFile, "space/enemy.bmp");
        
        gameState->Music = LoadWAV(memory->DEBUGPlatformReadEntireFile, "audio/music.wav");
        gameState->Gun = LoadWAV(memory->DEBUGPlatformReadEntireFile, "audio/gun.wav");
        gameState->Explosion = LoadWAV(memory->DEBUGPlatformReadEntireFile, "audio/explosion.wav");
        
        gameState->tSine = 0.0f;
        
        // set function pointers for Voidt module
        PlatformAddWorkEntry    = memory->PlatformAddWorkEntry;
        PlatformCompleteAllWork = memory->PlatformCompleteAllWork;

        // TODO(Joey): generate procedural world here
        InitializeArena(&gameState->WorldArena, Megabytes(32), (uint8*)memory->PermanentStorage + sizeof(game_state));        
        
        memory_arena mixerArena = {};
        InitializeArena(&mixerArena, Megabytes(1), gameState->WorldArena.Base + gameState->WorldArena.Size);
        gameState->Mixer.MixerArena = mixerArena;
        
        PlaySound(&gameState->Mixer, &gameState->Music);
        
        memory->IsInitialized = true;
    }            
    // NOTE(Joey): initialize transient memory: memory that to be reset/re-used each frame
    memory_arena transientArena; 
    InitializeArena(&transientArena, memory->TransientStorageSize, (uint8*)memory->TransientStorage);      
  
  
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

            if(controller->RightShoulder.EndedDown && gameState->FireDelay >= 0.25f)                
            {
                PlaySound(&gameState->Mixer, &gameState->Gun);
                gameState->FireDelay = 0.0f;
            }
            if (controller->LeftShoulder.EndedDown && gameState->ExplosionDelay >= 1.0f)
            {
                PlaySound(&gameState->Mixer, &gameState->Explosion);
                gameState->ExplosionDelay = 0.0f;
            }
        }
    }
    
    
    
    //////////////////////////////////////////////////////////
    //       RENDER
    //////////////////////////////////////////////////////////         
    Texture screenTexture = CreateEmptyTexture(&transientArena, screenBuffer->Width, screenBuffer->Height);
    RenderQueue *renderQueue = CreateRenderQueue(&transientArena, 256); 
        
    // background
    vector2D screenSize = { (real32)screenBuffer->Width, (real32)screenBuffer->Height };
    vector2D screenCenter = 0.5f*screenSize;
    PushTexture(renderQueue, 
                &gameState->Background, 
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
                &gameState->Player, 
                screenCenter + playerRelCamera, 
                0,
                { (real32)gameState->Player.Width, (real32)gameState->Player.Height },
                basisX,
                basisY,
                { 1.0f, 1.0f, 1.0f, 0.5f });

    // enemey
    vector2D enemyPos = { 350.0f, -150.0f };
    vector2D enemeyRelCamera = enemyPos - cameraPos;
    angle = 42.30f + gameState->TimePassed * 0.1f;
    basisX = Normalize({ (real32)cos(angle), (real32)sin(angle)});
    basisY = Perpendicular(basisX);
    PushTexture(renderQueue, 
                &gameState->Enemy,
                screenCenter + enemeyRelCamera,
                0,
                { 200.0f, 200.0f }, 
                basisX, 
                basisY, 
                { 1.0f, 1.0f, 1.0f, 1.0f });
                
    // render to target
    RenderPass(memory->WorkQueueHighPriority, renderQueue, &screenTexture);

        
    //////////////////////////////////////////////////////////
    //       OUTPUT 
    //////////////////////////////////////////////////////////
    BlitTextureToScreen(screenBuffer, &screenTexture);
    

    // PrintCPUTiming(0);
    // PrintCPUTiming(1);
    
    gameState->TimePassed += input->dtPerFrame;
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    // GameOutputSound(soundBuffer, gameState, 412);
    
    TempMemory mixResultMemory = BeginTempMemory(&gameState->Mixer.MixerArena);
    
    real32 *realChannel0 = PushArray(&gameState->Mixer.MixerArena, soundBuffer->SampleCount, real32);
    real32 *realChannel1 = PushArray(&gameState->Mixer.MixerArena, soundBuffer->SampleCount, real32);    
    for(int32 sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
    {
        realChannel0[sampleIndex] = 0.0f;
        realChannel1[sampleIndex] = 0.0f;
    }
    
    for(PlayingSound **playingSoundPtr = &gameState->Mixer.FirstPlayingSound;
        *playingSoundPtr;
        )
    {
        PlayingSound *playingSound = *playingSoundPtr;
        bool32 soundFinished = false;
        
        Sound* sound = playingSound->Source;
        if(sound)
        {
            real32 volume0 = playingSound->Volume[0];
            real32 volume1 = playingSound->Volume[1];
            real32 *channel0 = realChannel0;
            real32 *channel1 = realChannel1;
            
            Assert(playingSound->SamplesPlayed >= 0);
                        
            int32 samplesRemainingInSound = sound->SampleCount - playingSound->SamplesPlayed;
            int32 nrSamplesToMix = soundBuffer->SampleCount;
            if(!playingSound->Loop && samplesRemainingInSound < nrSamplesToMix)
                nrSamplesToMix = samplesRemainingInSound;
            
            for(int32 sampleIndex = playingSound->SamplesPlayed; 
                sampleIndex < (playingSound->SamplesPlayed + nrSamplesToMix); 
                ++sampleIndex)
            {        
                real32 sampleValue = sound->Samples[0][sampleIndex % sound->SampleCount];
                *channel0++ += volume0*sampleValue;
                *channel1++ += volume1*sampleValue;
            }
            
            playingSound->SamplesPlayed = playingSound->SamplesPlayed + nrSamplesToMix;
            if(playingSound->Loop)
                playingSound->SamplesPlayed = playingSound->SamplesPlayed % sound->SampleCount;
            
            soundFinished = !playingSound->Loop && (uint32)playingSound->SamplesPlayed == sound->SampleCount;       
        }
        else
        {
            // NOTE(Joey): Load sound here? or when retrieving from Asset manager; I'd say load sound when
            // not available in asset manager, much better path to take
        }
        
        if(soundFinished)
        {
            *playingSoundPtr = playingSound->Next;
            playingSound->Next = gameState->Mixer.FirstFreePlayingSound;
            gameState->Mixer.FirstFreePlayingSound = playingSound;
        }
        else
        {
            playingSoundPtr = &playingSound->Next;
        }
    }
      
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
