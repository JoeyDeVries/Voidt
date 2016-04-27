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
    
    int32 TempCount;
};


inline void InitializeArena(memory_arena *arena, memory_index size, void *base)
{
    arena->Size = size;
    arena->Base = (uint8*)base;
    arena->Used = 0;
    arena->TempCount = 0;
}

#define PushStruct(arena, type, ...) (type*)PushSize_(arena, sizeof(type), ## __VA_ARGS__)
#define PushArray(arena, count, type, ...) (type*)PushSize_(arena, (count)*sizeof(type), ## __VA_ARGS__)
inline void* PushSize_(memory_arena *arena, memory_index size, memory_index alignment = 4)
{       
    memory_index resultPointer = (memory_index)arena->Base + arena->Used;
    memory_index alignmentOffset = 0;
    
    memory_index alignmentMask = alignment - 1;
    if(resultPointer & alignmentMask)
        alignmentOffset = alignment - (resultPointer & alignmentMask);
    size += alignmentOffset;
    
    Assert(arena->Used + size <= arena->Size);
    arena->Used += size;
    
    void* address = (void*)(resultPointer + alignmentOffset);           
    return address;
}
#define ZeroStruct(instance) ZeroSize(sizeof(instance), &instance)
inline void ZeroSize(void *ptr, memory_index size)
{
    // TODO(Joey): check for performance
    uint8 *byte = (uint8*)ptr;
    for(memory_index i = 0; i < size; ++i)
    {
        *byte++ = 0;
    }
}

inline char* PushString(memory_arena *arena, char *string)
{    
    u32 size = 1; // NOTE(Joey): take 0 byte into account
    for(char *c = string; *c; ++c, ++size) { }
    
    char *mem = (char*)PushSize_(arena, size);
    for(u32 i = 0; i < size; ++i)
        mem[i] = string[i];
    
    return mem;
}


inline memory_arena SubArena(memory_arena *arena, memory_index size, memory_index alignment = 16)
{
    memory_arena result = {};
    result.Size = size;
    result.Base = (uint8*)PushSize_(arena, size, alignment);
    result.Used = 0;
    result.TempCount = 0;
    return result;
}

inline void
CheckArena(memory_arena *arena)
{
    Assert(arena->TempCount == 0);
}

// temp memory (stack-based allocation)
struct temp_memory
{
    memory_arena *Arena;
    memory_index  Used;
};

inline temp_memory BeginTempMemory(memory_arena *arena)
{
    temp_memory tempMemory;
    tempMemory.Arena = arena;
    tempMemory.Used = arena->Used;
    arena->TempCount++;
    return tempMemory;
}

inline void EndTempMemory(temp_memory tempMemory)
{
    Assert(tempMemory.Arena->Used >= tempMemory.Used);
    Assert(tempMemory.Arena->TempCount > 0);
    
    tempMemory.Arena->Used = tempMemory.Used;
    tempMemory.Arena->TempCount--;
}

///////////////////////////////////
//   GENERAL PURPOSE ALLOCATER
///////////////////////////////////
const memory_index BLOCK_SPLIT_THRESHOLD = 4096;

enum memory_block_flags
{
    MEMORY_BLOCK_USED = 0x1,    
};

struct memory_block 
{
    memory_block *Prev;
    memory_block *Next;
    
    memory_index Size;
    u64          Flags;
    
    void        *Memory;
};

struct general_purpose_allocater
{    
    memory_block Sentinel;
    
    memory_index TotalMemory;
    memory_index MemoryUsed;
};

#endif
