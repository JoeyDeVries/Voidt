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
#define ENTITY_h

#define INVALID_POS { 100000.0f, 100000.0f }

inline bool32 IsSet(sim_entity *entity, uint32 flag)
{
    bool32 result = entity->Flags & flag;
    return result;
}
inline void SetFlag(sim_entity *entity, uint32 flag)
{
    entity->Flags |= flag;
}
inline void ClearFlag(sim_entity *entity, uint32 flag)
{
    entity->Flags &= ~flag;
}

#endif