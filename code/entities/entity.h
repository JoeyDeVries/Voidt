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
#ifndef ENTITY_H
#define ENTITY_H

struct game_entity
{
    /* NOTE(Joey): Entity position & floating point precision:
      
      As this game has an (close to) infinite span representing entity
      position with a 32 bit floating point vector will get us into 
      precision issues. However, we will be using simulation regions
      that are small encapsulated areas where only entities inside
      those regions will get simulation updates.
      
      All simulated entities will have a temporary simulation position
      that is their delta between their position and the simulation 
      region's center. This delta will always be relatively small and
      have enough precision for the game's simulation.
    
    */
    vector2D Position; 
    vector2D Size;
    real32   Depth;
    u32      Type;
};

#endif