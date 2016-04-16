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

internal void PlaySound(SoundMixer *mixer, Sound *sound, real32 volume = 1.0f, bool32 loop = false)
{
    // NOTE(Joey): get new|free PlayingSound at top of linked list
    PlayingSound *playingSound = 0;
    if(mixer->FirstFreePlayingSound)
    {
        playingSound = mixer->FirstFreePlayingSound;
        mixer->FirstFreePlayingSound = playingSound->Next;
        playingSound->Next = 0;
    }
    else
    {
        playingSound = PushStruct(&mixer->MixerArena, PlayingSound);        
    }
    playingSound->Next = mixer->FirstPlayingSound;
    mixer->FirstPlayingSound = playingSound;
    
    // NOTE(Joey): initialize playing sound
    playingSound->Source = sound; 
    playingSound->SamplesPlayed = 0;
    playingSound->Loop = loop;
    playingSound->Volume[0] = volume;
    playingSound->Volume[1] = volume;            
}


internal void MixSounds(SoundMixer *mixer, real32 *realChannel0, real32 *realChannel1, int32 sampleCount)
{
    // NOTE(Joey): clean channels with 0.0f before mixing
    for(int32 sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        realChannel0[sampleIndex] = 0.0f;
        realChannel1[sampleIndex] = 0.0f;
    }
    
    // NOTE(Joey): loop over all playing sounds and mix into both real floating point channels
    for(PlayingSound **playingSoundPtr = &mixer->FirstPlayingSound;
        *playingSoundPtr;
        )
    {
        PlayingSound *playingSound = *playingSoundPtr;
        bool32 soundFinished = false;
        
        Sound* sound = playingSound->Source;
        if(sound)
        {
            real32 volume0 = playingSound->Volume[0];
            real32 volume1 = playingSound->Volume[1];
            real32 *channel0 = realChannel0;
            real32 *channel1 = realChannel1;
            
            Assert(playingSound->SamplesPlayed >= 0);
                        
            int32 samplesRemainingInSound = sound->SampleCount - playingSound->SamplesPlayed;
            int32 nrSamplesToMix = sampleCount;
            if(!playingSound->Loop && samplesRemainingInSound < nrSamplesToMix)
                nrSamplesToMix = samplesRemainingInSound;
            
            for(int32 sampleIndex = playingSound->SamplesPlayed; 
                sampleIndex < (playingSound->SamplesPlayed + nrSamplesToMix); 
                ++sampleIndex)
            {        
                real32 sampleValue = sound->Samples[0][sampleIndex % sound->SampleCount];
                *channel0++ += volume0*sampleValue;
                *channel1++ += volume1*sampleValue;
            }
            
            playingSound->SamplesPlayed = playingSound->SamplesPlayed + nrSamplesToMix;
            if(playingSound->Loop)
                playingSound->SamplesPlayed = playingSound->SamplesPlayed % sound->SampleCount;
            
            soundFinished = !playingSound->Loop && (uint32)playingSound->SamplesPlayed == sound->SampleCount;       
        }
        else
        {
            // NOTE(Joey): Load sound here? or when retrieving from Asset manager; I'd say load sound when
            // not available in asset manager, much better path to take
        }
        
        if(soundFinished)
        {
            *playingSoundPtr = playingSound->Next;
            playingSound->Next = mixer->FirstFreePlayingSound;
            mixer->FirstFreePlayingSound = playingSound;
        }
        else
        {
            playingSoundPtr = &playingSound->Next;
        }
    }
}



void OutputTestSineWave(game_sound_output_buffer *soundBuffer, int toneHz)
{
    local_persist real32 tSine = 0.0f;
    uint16 toneVolume = 3000;
    int wavePeriod = soundBuffer->SamplesPerSecond / toneHz;
    
    int16 *sampleOut = soundBuffer->Samples;
    for(int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
    {
        real32 sineValue = sinf(tSine);
        int16 sampleValue = (int16)(sineValue * toneVolume);
        // sampleValue = 0; // disable
        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;
        
        tSine += (2.0f * Pi32) / (real32)wavePeriod;
        
        if(tSine >= 2.0f * Pi32)
            tSine -= 2.0f * Pi32;
    }        
} 