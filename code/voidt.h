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
#include "common/memory.h"
#include "voidt_platform.h"
#include "intrinsics.h"
#include "math/math.h"
// #include "map.h"
// #include "sim_region.h"
// #include "entity.h"

#include "common/random.h"
#include "common/random.cpp"
#include "renderer/renderer.h"
#include "renderer/renderer.cpp"


inline game_controller_input* GetController(game_input *input, uint32 controllerIndex)
{
    Assert(controllerIndex < ArrayCount(input->Controllers));
    game_controller_input *result = &input->Controllers[controllerIndex];
    return result;
}


// struct game_entity
// {      
    // uint32 LowIndex;
    // high_entity *High;
    // low_entity *Low;
// };


struct controlled_player
{
    uint32 EntityIndex;
    vector3D Velocity;
    vector2D Acceleration;
    vector2D AccelerationSword;
    // real32 dZ;
};

struct game_state
{
    memory_arena WorldArena;       
    controlled_player ControlledPlayers[ArrayCount(((game_input*)0)->Controllers)];

    Texture Player;
    Texture Enemy;
    Texture Background;
    
    real32 TimePassed;
};

#endif