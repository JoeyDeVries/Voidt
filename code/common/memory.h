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
#ifndef MEMROY_H
#define MEMROY_H

struct memory_arena 
{
    uint8 *Base;
    memory_index Size;
    memory_index Used;
};


inline void InitializeArena(memory_arena *arena, memory_index size, void *base)
{
    arena->Size = size;
    arena->Base = (uint8*)base;
    arena->Used = 0;
}

#define PushStruct(arena, type) (type*)PushSize_(arena, sizeof(type))
#define PushArray(arena, count, type) (type*)PushSize_(arena, (count)*sizeof(type))
inline void* PushSize_(memory_arena *arena, memory_index size)
{
    Assert(arena->Used + size <= arena->Size);
    void* address = arena->Base + arena->Used;
    arena->Used += size;
    
    // clear to zero (redundant for now)    
    // ZeroSize(size, address);
    // for(uint8* memory = (uint8*)address; memory < arena->Base + arena->Used + size; ++memory)
        // *memory = 0;
    
    return address;
}
#define ZeroStruct(instance) ZeroSize(sizeof(instance), &instance)
inline void ZeroSize(void *ptr, memory_index size)
{
    // TODO(Joey): check for performance
    uint8 *byte = (uint8*)ptr;
    while(size--)
    {
        *byte++ = 0;
    }
}


#endif
