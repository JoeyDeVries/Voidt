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
#ifndef TIMING_H
#define TIMING_H

struct CPUTiming
{
    uint64 StartCycle;
    uint64 TotalCycles;
    uint32 HitCount;
};

global_variable CPUTiming _CPUPerformanceBuffer[16]; 



#define TIMING_BLOCK__(Number, ...) timing_block TimingBlock_##Number(__COUNTER__, __FILE__,__FUNCTION__,__LINE__,## __VA_ARGS__)
#define TIMING_BLOCK_(Number, ...) TIMING_BLOCK__(Number, ## __VA_ARGS__)
#define TIMING_BLOCK(...) TIMING_BLOCK_(__LINE__, ## __VA_ARGS__)

struct timing_record
{
    u64 CycleCount;
    u32 LineNumber;
    u32 HitCount;
    
    // NOTE(Joey): store strings from pre-processor; string values directly
    // assigned to a pointer (which we do here using the pre-processor) are 
    // stored in data segment and is shared among functions. 
    // See http://www.geeksforgeeks.org/storage-for-strings-in-c/ This is thus
    // perfectly fine to do and store directly in global array.
    char *FileName;
    char *FunctionName;
};

// NOTE(Joey): forward declarate from array at bottom of voidt.cpp
timing_record TimingRecords[];

struct timing_block
{
    timing_record *m_Record;
    u64            m_StartCycles;
    
    timing_block(int counter, char *fileName, char *function, int line, int count = 1)
    {       
        m_Record = TimingRecords + counter;            
        
        _InterlockedExchangeAdd((volatile long *)&m_Record->HitCount, count);
        
        m_Record->FileName     = fileName;
        m_Record->FunctionName = function;
        m_Record->LineNumber   = line;
        
        // NOTE(Joey): set CycleCount to negative s.t. adding it at the end results in the (positive) difference
        m_StartCycles = __rdtsc();
    }
    
    ~timing_block()
    {
        u64 delta = __rdtsc() - m_StartCycles;
        _InterlockedExchangeAdd((volatile long *)&m_Record->CycleCount, (long)delta);
    }
};

#endif