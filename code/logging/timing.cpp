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

void BeginCPUTiming(uint32 id)
{
    Assert(id < ArrayCount(_CPUPerformanceBuffer));
    
    ++_CPUPerformanceBuffer[id].HitCount;
    _CPUPerformanceBuffer[id].StartCycle = __rdtsc();
}

void EndCPUTiming(uint32 id, uint32 workCount = 1)
{
    Assert(id < ArrayCount(_CPUPerformanceBuffer));    
    
    __int64 endCycleCount = __rdtsc();
    _CPUPerformanceBuffer[id].TotalCycles += ((endCycleCount - _CPUPerformanceBuffer[id].StartCycle) / workCount);
}

void PrintCPUTiming(uint32 id)
{
    Assert(id < ArrayCount(_CPUPerformanceBuffer));
    
    PlatformAPI.WriteDebugOutput("CPU-ID: %i | cycles: %llu | hits: %d | cycles/hit: %llu\n",
                                 id, _CPUPerformanceBuffer[id].TotalCycles, _CPUPerformanceBuffer[id].HitCount, 
                                 _CPUPerformanceBuffer[id].TotalCycles / _CPUPerformanceBuffer[id].HitCount); 
    _CPUPerformanceBuffer[id] = {};
}


