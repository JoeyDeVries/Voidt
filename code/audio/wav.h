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
// see: http://www-mmsp.ece.mcgill.ca/documents/audioformats/wave/wave.html

#define RIFF_CODE(a, b, c, d) (((uint32)(a) << 0) | ((uint32)(b) << 8) | ((uint32)(c) << 16) | ((uint32)(d) << 24))
enum
{
    WAVE_ID_FMT  = RIFF_CODE('f', 'm', 't', ' '),
    WAVE_ID_DATA = RIFF_CODE('d', 'a', 't', 'a'),
    WAVE_ID_RIFF = RIFF_CODE('R', 'I', 'F', 'F'),
    WAVE_ID_WAVE = RIFF_CODE('W', 'A', 'V', 'E'),
};

#pragma pack(push, 1)
struct WAVEHeader 
{
    uint32 RIFFID;
    uint32 Size;
    uint32 WAVEID;
};

struct WAVEChunk
{
    uint32 WaveID;
    uint32 Size;
};

struct WAVEFormat 
{
    uint16 WFormatTag;
    uint16 NChannels;
    uint32 NSamplesPerSec;
    uint32 NAvgBytesPerSec;
    uint16 NBlockAlign;
    uint16 WBitsPerSample;
    uint16 CbSize;
    uint16 WValidBitsPerSample;
    uint32 DwChannelMask;
    uint8  SubFormat[16];    
};
#pragma pack(pop)