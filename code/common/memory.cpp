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

internal memory_block* InsertMemoryBlock(memory_block *prev, memory_index size, void *memory);

internal general_purpose_allocater* GenerateGeneralPurposeAllocater(memory_arena *arena, memory_index size)
{
    general_purpose_allocater *result = PushStruct(arena, general_purpose_allocater);
    
    result->MemoryUsed = 0;       
    result->TotalMemory = size;
    
    result->Sentinel.Flags = 0;
    result->Sentinel.Size = 0;
    result->Sentinel.Prev = &result->Sentinel;
    result->Sentinel.Next = &result->Sentinel;
    result->Sentinel.Memory = 0;
    
    // NOTE(Joey): we allocate the entire memory range to the memory sentinel
    InsertMemoryBlock(&result->Sentinel, size, PushSize_(arena, size));
    
    return result;
}

internal memory_block* InsertMemoryBlock(memory_block *prev, memory_index size, void *memory)
{
    Assert(size > sizeof(memory_block)); // NOTE(Joey): make sure there is always room for the header
    
    memory_block *block = (memory_block*)memory;
    block->Flags = 0;
    block->Size = size - sizeof(memory_block);
    block->Prev = prev;
    block->Next = prev->Next;
    block->Memory = (u8*)memory + sizeof(memory_block);
    
    block->Prev->Next = block;
    block->Next->Prev = block;
    
    return block;
}

internal memory_block* FindBlockWithSize(general_purpose_allocater *allocator, memory_index size)
{
    memory_block *result = 0;
    
    for(memory_block *block = allocator->Sentinel.Next;
        block != &allocator->Sentinel;
        block = block->Next)
    {
        if(!(block->Flags & MEMORY_BLOCK_USED))
        {
            if(block->Size >= size)
            {
                result = block;
                break;
            }
        }
    }
    return result;
}

internal b32 MergeMemoryBlocks(general_purpose_allocater *allocator, memory_block *first, memory_block *second)
{
    b32 result = false;
    
    if(first != &allocator->Sentinel && second != &allocator->Sentinel)
    {
        if(!(first->Flags & MEMORY_BLOCK_USED) && !(second->Flags & MEMORY_BLOCK_USED))
        {
            // NOTE(Joey): get the memory location of the second block next to the first block
            u8 *memorySecondBlock = (u8*)first + sizeof(memory_block) + first->Size;
            if((u8*)second == memorySecondBlock)
            {
                second->Next->Prev = second->Prev;
                first->Next = second->Next;

                first->Size += sizeof(memory_block) + second->Size;
                
                result = true;
            }
        }
    }
    return result;
}

/* NOTE(Joey):

  Returns a pointer to free general memory from the general purpose
  memory allocator pool. This function automatically splits or 
  merges the memory based on the pool's fragmentation.
    
  To keep the system general and not implementation-dependent
  we return the memory_blocks itself that can later be sent back
  to the allocator for de-allocation. This way, the application
  logic can decide when to free the stored memory.

*/
internal memory_block* GetGeneralMemory(general_purpose_allocater *allocator, memory_index size)
{
    memory_block *block = FindBlockWithSize(allocator, size);    
    
    // memory_block *foundBlock = FindBlockWithSize(allocator, size);    
    // NOTE(Joey): if there is a block found of given size
    if(block && size <= block->Size)
    {
        block->Flags |= MEMORY_BLOCK_USED;
        allocator->MemoryUsed += size;
        
        block->Memory = (u8*)(block + sizeof(memory_block));
        
        memory_index remainingSize = block->Size - size;
        // NOTE(Joey): if remaining size is above split threshold; split the block
        if(remainingSize > BLOCK_SPLIT_THRESHOLD)
        {
            block->Size -= remainingSize;
            InsertMemoryBlock(block, remainingSize, (u8*)block->Memory + size);
        }
        else
        {
            // NOTE(Joey): record unused portion of the memory in a block so 
            // we can later do a merge on blocks when neighbors are freed.
        }                
        
    }
    // NOTE(Joey): else try and merge unused blocks to make space
    else
    {
        for(memory_block *memoryBlock = allocator->Sentinel.Next;
            memoryBlock != &allocator->Sentinel;
            memoryBlock = memoryBlock->Next)
        {
            if(!(memoryBlock->Flags & MEMORY_BLOCK_USED))
            {
                if(MergeMemoryBlocks(allocator, memoryBlock->Prev, memoryBlock))
                    memoryBlock = memoryBlock->Prev;
                MergeMemoryBlocks(allocator, memoryBlock, memoryBlock->Next);
                
                if(memoryBlock->Size >= size)
                {
                    block = memoryBlock;
                    block->Flags |= MEMORY_BLOCK_USED;
                    block->Memory = (u8*)(block + sizeof(memory_block));
                    
                    allocator->MemoryUsed += size;
                    break;
                }
            }
        }        
    }
    Assert(block); // NOTE(Joey): if block is null pointer we need better de-fragmentation
    return block;
}

internal void *FreeGeneralMemory(general_purpose_allocater *allocator, memory_block *block)
{
    allocator->MemoryUsed -= block->Size;
    block->Flags &= ~MEMORY_BLOCK_USED;    
    
    // NOTE(Joey): merge with neighbors if possible
    if(MergeMemoryBlocks(allocator, block->Prev, block))
        block = block->Prev;
    MergeMemoryBlocks(allocator, block, block->Next);
}
