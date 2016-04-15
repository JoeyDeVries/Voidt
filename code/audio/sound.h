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
#ifndef SOUND_H
#define SOUND_H

struct Sound 
{
    uint32 ChannelCount;
    uint32 SampleCount;
    int16 *Samples[2];           
};

struct Mixer 
{
    uint32 RunningSampleIndex;
    
    Sound SoundsPlaying[16];
};

#include "wav.h"
#include "wav.cpp"

#endif