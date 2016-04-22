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

global_variable random_series GlobalRandom;

inline random_series Seed(u32 value, u32 modulo = 65536)
{
    random_series series = {};
    
    series.Index = value;
    series.Modulo = modulo;
    return series;
}

inline uint32 RandomNextUInt32(random_series *series)
{
    // PRNG: Linear Congruential Generator 
    u32 a = 1103515245;
    u32 c = 12345;
    series->Index = (a * series->Index + c) % series->Modulo;
    return series->Index;
}

inline uint32 RandomChoice(random_series *series, uint32 count)
{
    uint32 result = RandomNextUInt32(series) % count;
    return result;
}

inline real32 RandomUniliteral(random_series *series)
{
    real32 divisor = 1.0f / series->Modulo;
    real32 result = divisor*(real32)RandomNextUInt32(series);
    return result;
}

inline real32 RandomBiliteral(random_series *series)
{
    real32 result = 2.0f * RandomUniliteral(series) - 1.0f;
}

inline real32 RandomBetween(random_series *series, real32 min, real32 max)
{
    real32 result = Lerp(min, max, RandomUniliteral(series));
    return result;
}

inline int32 RandomBetween(random_series *series, int32 min, int32 max)
{
    int32 result = min + RandomNextUInt32(series) % ((max + 1) - min);
    return result;
}