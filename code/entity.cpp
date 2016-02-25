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
inline move_spec DefaultMoveSpec()
{
    move_spec result;
    
    result.UnitMaxAccelVector = false;
    result.Speed = 1.0f;
    result.Drag = 0.0f;
    
    return result;
}

internal void UpdateFamiliar(sim_region *simRegion, sim_entity *entity, real32 dt)
{
    sim_entity *closestPlayer = 0;
    real32 closestPlayerDSq = Square(10.0f); // NOTE(Joey): don't search for more than 10 meters
    sim_entity *testEntity = simRegion->Entities;
    for(uint32 testEntityIndex = 1; testEntityIndex < simRegion->EntityCount; ++testEntityIndex)
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
    
    vector2D acceleration = {};
    if(closestPlayer && closestPlayerDSq > Square(3.0f))
    {
        real32 speed = 0.5f;
        real32 oneOverLength = speed / SquareRoot(closestPlayerDSq);
        acceleration = oneOverLength * (closestPlayer->Position - entity->Position);
        
    }
    
    move_spec moveSpec = DefaultMoveSpec();
    moveSpec.UnitMaxAccelVector = true;
    moveSpec.Speed = 50.0f;
    moveSpec.Drag = 8.0f;
    
    MoveEntity(simRegion, entity, dt, &moveSpec, acceleration);
}

internal void UpdateMonster(sim_region *simRegion, sim_entity *entity, real32 dt)
{
    
}

internal void UpdateSword(sim_region *simRegion, sim_entity *entity, real32 dt)
{
    move_spec moveSpec = DefaultMoveSpec();
    moveSpec.UnitMaxAccelVector = false;
    moveSpec.Speed = 0.0f;
    moveSpec.Drag = 0.0f;
    
    vector2D oldPos = entity->Position;
    MoveEntity(simRegion, entity, dt, &moveSpec, { 0, 0 });
    real32 distanceTraveled = Length(entity->Position - oldPos);
    
    entity->DistanceRemaining -= distanceTraveled;
    if(entity->DistanceRemaining < 0.0f)
    {
        Assert(!"TODO: BUILD SYSTEM TO REMOVE ENTITIES FROM SIMULATION");
    }
}
