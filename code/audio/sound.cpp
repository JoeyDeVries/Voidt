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

internal PlayingSound* PlaySound(SoundMixer *mixer, Sound *sound, real32 volume = 1.0f, real32 pitch = 1.0f, bool32 loop = false)
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
    playingSound->CurrentVolume[0] = playingSound->TargetVolume[0] = volume;
    playingSound->CurrentVolume[1] = playingSound->TargetVolume[1] = volume;      
    playingSound->dVolume[0] = playingSound->dVolume[1] = 0.0f;    
    playingSound->Pitch = pitch;
    
    return playingSound;
}


internal void MixSounds(SoundMixer *mixer, uint32 sampleFrequency, __m128 *realChannel0, __m128 *realChannel1, int32 sampleCount)
{
    i32 sampleCount4 = sampleCount / 4;
    
    // simd globals
    __m128 zero = _mm_set1_ps(0.0f);
    __m128 minS16 = _mm_set1_ps(32767.0f);
    __m128 maxS16 = _mm_set1_ps(-32768.0f);
    
    // get number of samples per second
    real32 secondsPerSample = 1.0f / (real32)sampleFrequency;
    
    // NOTE(Joey): clean channels with 0.0f before mixing
    for(int32 sampleIndex = 0; sampleIndex < sampleCount4; ++sampleIndex)
    {
        _mm_store_ps((float*)(realChannel0 + sampleIndex), zero);
        _mm_store_ps((float*)(realChannel1 + sampleIndex), zero);
         // = zero;
        // realChannel1[sampleIndex] = zero;
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
            r32 volume0 = playingSound->CurrentVolume[0];
            r32 volume1 = playingSound->CurrentVolume[1];
            r32 dVolume0 = secondsPerSample*playingSound->dVolume[0];
            r32 dVolume1 = secondsPerSample*playingSound->dVolume[1];
            r32 *channel0 = (real32*)realChannel0;
            r32 *channel1 = (real32*)realChannel1;            
            r32 dSample = playingSound->Pitch;
            
            Assert(playingSound->SamplesPlayed >= 0);
                        
            // NOTE(Joey): determine the maximum number of samples to mix this frame
            i32 nrSamplesToMix = sampleCount;
            // NOTE(Joey): deltaSampleRates can get negative (likely due to floating point precision)
            // this is accounted for in loop condition.
            i32 deltaSampleRates = (sound->SampleCount - RoundReal32ToInt32(playingSound->SamplesPlayed));
            r32 realSamplesRemainingInSound = (r32)deltaSampleRates / dSample;
            i32 samplesRemainingInSound = RoundReal32ToInt32(realSamplesRemainingInSound);
            if(!playingSound->Loop && samplesRemainingInSound <= (i32)nrSamplesToMix)
                nrSamplesToMix = samplesRemainingInSound;
            
            
            // NOTE(Joey): determine if we need to break out of the loop early due to volume
            // attenuation reaching 0.0f volume per channel.
            bool32 volumeEnded[2] = {};
            // TODO(Joey): make logic independent of nummer of channels
            if(dVolume0 != 0.0f)
            {
                r32 deltaVolume = playingSound->TargetVolume[0] - volume0;
                i32 volumeSampleCount = (u32)((deltaVolume / dVolume0) + 0.5f);
                if(volumeSampleCount <= nrSamplesToMix)
                {
                    nrSamplesToMix = volumeSampleCount;
                    volumeEnded[0] = true;
                }
            }
            if(dVolume1 != 0.0f)
            {
                r32 deltaVolume = playingSound->TargetVolume[1] - volume1;
                i32 volumeSampleCount = (u32)((deltaVolume / dVolume1) + 0.5f);
                if(volumeSampleCount <= nrSamplesToMix)
                {
                    nrSamplesToMix = volumeSampleCount;
                    volumeEnded[1] = true;
                }
            }
            
            r32 samplePosition = playingSound->SamplesPlayed;
            for(i32 i = 0; i < nrSamplesToMix; ++i)
            {                        
                u32 sampleIndex = FloorReal32ToInt32(samplePosition);
                r32 frac = samplePosition - (r32)sampleIndex;
                r32 sample0 = (r32)sound->Samples[0][sampleIndex % sound->SampleCount];
                r32 sample1 = (r32)sound->Samples[0][sampleIndex + 1 % sound->SampleCount];
                r32 sampleValue = Lerp(sample0, sample1, frac);
                
                *channel0++ += volume0*sampleValue;
                *channel1++ += volume1*sampleValue;
                
                volume0 += dVolume0;
                volume1 += dVolume1;
                
                samplePosition += dSample;
            }
            
            playingSound->SamplesPlayed = samplePosition;
            playingSound->CurrentVolume[0] = volume0;
            playingSound->CurrentVolume[1] = volume1;
            
            // NOTE(Joey): if volume 0.0f is reached due to attenuation, reset delta volume
            for(int32 i = 0; i < ArrayCount(volumeEnded); ++i)
            {
                if(volumeEnded[i])
                {
                    playingSound->CurrentVolume[i] = playingSound->TargetVolume[i];
                    playingSound->dVolume[i] = 0.0f;
                }
            }
            
            // if loop, re-position SamplesPlayed to start of sound sample0
            if(playingSound->Loop && playingSound->SamplesPlayed > (r32)sound->SampleCount)
               playingSound->SamplesPlayed = playingSound->SamplesPlayed - (r32)sound->SampleCount;
          
            
            soundFinished = !playingSound->Loop && (uint32)playingSound->SamplesPlayed == sound->SampleCount; 
            if(soundFinished)
                volume0 = 0.0f;
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

void SetVolume(PlayingSound *sound, real32 volume0, real32 volume1, real32 fadeDuration = 0.0f)
{
    if(fadeDuration <= 0.0f)
    {   
        // NOTE(Joey): if no fade duration is specified; directly set both volume channels
        sound->CurrentVolume[0] = sound->TargetVolume[0] = volume0;
        sound->CurrentVolume[1] = sound->TargetVolume[1] = volume1;
    }
    else
    {
        sound->TargetVolume[0] = volume0;
        sound->TargetVolume[1] = volume1;
        
        real32 oneOverDuration = 1.0f / fadeDuration;
        sound->dVolume[0] = oneOverDuration * (sound->TargetVolume[0] - sound->CurrentVolume[0]);
        sound->dVolume[1] = oneOverDuration * (sound->TargetVolume[1] - sound->CurrentVolume[1]);
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