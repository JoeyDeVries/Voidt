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
   
}

internal void UpdateMonster(sim_region *simRegion, sim_entity *entity, real32 dt)
{
    
}

inline void MakeEntityNonSpatial(sim_entity *entity)
{
    SetFlag(entity, ENTITY_FLAG_NONSPATIAL);
    entity->Position = INVALID_POS;
}

inline void MakeEntitySpatial(sim_entity *entity, vector3D pos, vector3D velocity)
{
    ClearFlag(entity, ENTITY_FLAG_NONSPATIAL);
    entity->Position = pos;
    entity->Velocity = velocity;
}

internal void UpdateSword(sim_region *simRegion, sim_entity *entity, real32 dt)
{
   
}
