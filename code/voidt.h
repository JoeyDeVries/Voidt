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
#ifndef VOIDT_H
#define VOIDT_H

#include "common/voidt_common.h"
#include "common/platform_work_queue.h"
#include "voidt_platform.h"
#include "common/memory.h"
#include "intrinsics.h"
#include "common/random.h"
#include "math/math.h"

#include "logging/cpu_timing.h"
#include "logging/cpu_timing.cpp"

#include "common/random.cpp"
#include "renderer/renderer.h"
#include "renderer/renderer.cpp"
#include "audio/sound.h"
#include "audio/sound.cpp"

#include "common/asset.h"
#include "common/asset.cpp"



inline game_controller_input* GetController(game_input *input, uint32 controllerIndex)
{
    Assert(controllerIndex < ArrayCount(input->Controllers));
    game_controller_input *result = &input->Controllers[controllerIndex];
    return result;
}


struct controlled_player
{
    uint32 EntityIndex;
    vector3D Velocity;
    vector2D Acceleration;
    vector2D AccelerationSword;
};

struct game_state
{
    bool32 IsInitialized;
    memory_arena WorldArena;       
    
    controlled_player ControlledPlayers[ArrayCount(((game_input*)0)->Controllers)];

    real32 TimePassed;
    vector2D CameraPos;
    
    // audio/sound
    SoundMixer Mixer;
   
    // debug
    real32 FireDelay;
    real32 ExplosionDelay;
};

struct TransientState 
{
    bool32 IsInitialized;
    
    memory_arena TransientArena;
        
    // assets
    GameAssets Assets;        
};


#endif