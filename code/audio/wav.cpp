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

struct riff_iterator
{
    uint8 *At;
    uint8 *Stop;
};

inline riff_iterator ParseChunkAt(void *at, void *stop)
{
    riff_iterator iter;

    iter.At = (uint8 *)at;
    iter.Stop = (uint8 *)stop;

    return(iter);
}

inline riff_iterator NextChunk(riff_iterator iter)
{
    WAVEChunk *chunk = (WAVEChunk *)iter.At;
    uint32 size = (chunk->Size + 1) & ~1;
    iter.At += sizeof(WAVEChunk) + size;

    return(iter);
}
             
inline bool32 IsValid(riff_iterator iter)
{    
    bool32 result = (iter.At < iter.Stop);
    
    return(result);
}

inline void *GetChunkData(riff_iterator iter)
{
    void *result = (iter.At + sizeof(WAVEChunk));

    return(result);
}

inline uint32 GetType(riff_iterator iter)
{
    WAVEChunk *chunk = (WAVEChunk*)iter.At;
    uint32 result = chunk->WaveID;

    return(result);
}

inline uint32 GetChunkDataSize(riff_iterator iter)
{
    WAVEChunk *chunk = (WAVEChunk*)iter.At;
    uint32 result = chunk->Size;

    return(result);
}

internal Sound LoadWAV(debug_platform_read_entire_file *readEntireFile, char *fileName)
{
    Sound result = {};
    
    debug_read_file_result readResult = readEntireFile(fileName);    
    if(readResult.ContentSize != 0)
    {
        WAVEHeader *header = (WAVEHeader*)readResult.Contents;
        Assert(header->RIFFID == WAVE_ID_RIFF);
        Assert(header->WAVEID == WAVE_ID_WAVE);        
        
        uint32 channelCount = 0;
        uint32 sampleDataSize = 0;
        int16 *sampleData = 0;
        for(riff_iterator iter = ParseChunkAt(header + 1, (uint8 *)(header + 1) + header->Size - 4);
            IsValid(iter);
            iter = NextChunk(iter))
        {
            switch(GetType(iter))
            {
                case WAVE_ID_FMT:
                {
                    WAVEFormat *fmt = (WAVEFormat*)GetChunkData(iter);
                    Assert(fmt->WFormatTag == 1); // NOTE(Joey): only support PCM WAV files
                    Assert(fmt->NSamplesPerSec == 48000);
                    Assert(fmt->WBitsPerSample == 16);
                    Assert(fmt->NBlockAlign == (sizeof(int16)*fmt->NChannels));
                    channelCount = fmt->NChannels;
                } break;

                case WAVE_ID_DATA:
                {
                    sampleData = (int16 *)GetChunkData(iter);
                    sampleDataSize = GetChunkDataSize(iter);
                } break;

                case WAVE_ID_RIFF:
                {
                    int x = 5;
                    break;
                }
                case WAVE_ID_WAVE:
                {
                    int x = 5;
                    break;
                }
            }
        }

        Assert(channelCount && sampleData);

        result.ChannelCount = channelCount;
        result.SampleCount = sampleDataSize / (channelCount*sizeof(int16));
        if(channelCount == 1)
        {
            result.Samples[0] = sampleData;
            result.Samples[1] = 0;
        }
        else if(channelCount == 2)
        {
            result.Samples[0] = sampleData;
            result.Samples[1] = sampleData + result.SampleCount;
            
            for(uint32 sampleIndex = 0;
                sampleIndex < result.SampleCount;
                ++sampleIndex)
            {
                // TODO(Joey): work on proper 2-channel support
                int16 source = sampleData[2*sampleIndex];
                sampleData[2*sampleIndex] = sampleData[sampleIndex];
                sampleData[sampleIndex] = source;
            }
        }
        else
        {
            Assert(!"Invalid channel count in WAV file");
        }
        // NOTE(Joey): enforce 1 channel support for now
        result.ChannelCount = 1;
    }

    return(result);    
}