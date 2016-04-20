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

internal void InitSoundMixer(SoundMixer *mixer)
{
    mixer->MasterVolume = 1.0f;
}

internal void MixSounds(SoundMixer *mixer, uint32 sampleFrequency, __m128 *realChannel0, __m128 *realChannel1, int32 sampleCount)
{
    i32 sampleCount4 = sampleCount / 4;
    
    // simd globals
    __m128 zero = _mm_set1_ps(0.0f);
    __m128 one  = _mm_set1_ps(1.0f);

    // get number of samples per second
    r32 secondsPerSample = 1.0f / (r32)sampleFrequency;
    
    // NOTE(Joey): clean channels with 0.0f before mixing
    for(int32 sampleIndex = 0; sampleIndex < sampleCount4; ++sampleIndex)
    {
        _mm_store_ps((float*)(realChannel0 + sampleIndex), zero);
        _mm_store_ps((float*)(realChannel1 + sampleIndex), zero);
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
            r32 dSample = playingSound->Pitch;
            
            __m128 *channel0 = realChannel0;
            __m128 *channel1 = realChannel1;            
            __m128 masterVolume4 = _mm_set1_ps(mixer->MasterVolume);
            __m128 volume4_0 = _mm_setr_ps(volume0 + 0.0f*dVolume0,
                                           volume0 + 1.0f*dVolume0,
                                           volume0 + 2.0f*dVolume0,
                                           volume0 + 3.0f*dVolume0);
            __m128 volume4_1 = _mm_setr_ps(volume1 + 0.0f*dVolume1,
                                           volume1 + 1.0f*dVolume1,
                                           volume1 + 2.0f*dVolume1,
                                           volume1 + 3.0f*dVolume1);
            __m128 dVolume4_0 = _mm_set1_ps(4.0f*dVolume0);
            __m128 dVolume4_1 = _mm_set1_ps(4.0f*dVolume1);
            
            Assert(playingSound->SamplesPlayed >= 0);
                        
            // NOTE(Joey): determine the maximum number of samples to mix this frame
            i32 nrSamplesToMix = sampleCount;
            // NOTE(Joey): deltaSampleRates can get negative (likely due to floating point precision)
            // this is accounted for in loop condition.
            i32 deltaSampleRates = (sound->SampleCount - RoundReal32ToInt32(playingSound->SamplesPlayed));
            r32 realSamplesRemainingInSound = (r32)deltaSampleRates / (1.0f*dSample);
            i32 samplesRemainingInSound = RoundReal32ToInt32(realSamplesRemainingInSound);
            if(!playingSound->Loop && samplesRemainingInSound <= (i32)nrSamplesToMix)
            {
                nrSamplesToMix = samplesRemainingInSound;
            }
            
            
            
            // NOTE(Joey): determine if we need to break out of the loop early due to volume
            // attenuation reaching 0.0f volume per channel.
            bool32 volumeEnded[2] = {};
            // TODO(Joey): make logic independent of nummer of channels
            // NOTE(Joey): could have a bug here where the volumeEnded (should) get triggered
            // for both volumes at the same time; which due to this logic will only work on one
            // volume item. I don't think this is a problem, but if so - this is likely it.
            if(dVolume0 != 0.0f)
            {
                r32 deltaVolume = playingSound->TargetVolume[0] - playingSound->CurrentVolume[0];
                i32 volumeSampleCount = (i32)((deltaVolume / (1.0f*dVolume0)) + 0.5f);
                if(volumeSampleCount <= nrSamplesToMix)
                {
                    nrSamplesToMix = volumeSampleCount;
                    volumeEnded[0] = true;
                }
            }
            if(dVolume1 != 0.0f)
            {
                r32 deltaVolume = playingSound->TargetVolume[1] - playingSound->CurrentVolume[1];
                i32 volumeSampleCount = (u32)((deltaVolume / (1.0f*dVolume1)) + 0.5f);
                if(volumeSampleCount <= nrSamplesToMix)
                {
                    nrSamplesToMix = volumeSampleCount;
                    volumeEnded[1] = true;
                }
            }
            
            // NOTE(Joey): we get into float precision issues; take expected begin/end sample 
            // position and set playingSound->SamplePosition to expected end sample position.
            // then get next sample position in the loop loop index and start sample position.
            r32 beginSamplePosition = playingSound->SamplesPlayed;
            r32 endSamplePosition = beginSamplePosition + nrSamplesToMix*dSample;
            r32 loopIndexC = (endSamplePosition - beginSamplePosition) / (r32)nrSamplesToMix;
            // r32 samplePosition = playingSound->SamplesPlayed;
            // NOTE(Joey): clamp nrSamplesToMix loop condition to SIMD width of 4
            for(i32 i = 0; i < nrSamplesToMix - (nrSamplesToMix & 3); i += 4)
            {                   
                r32 samplePosition = beginSamplePosition + loopIndexC*(r32)i;
#if 0 // linear filtering // NOTE(Joey): disabled for now as it doesn't seem to make an 'audible' difference
                __m128 samplePos = _mm_setr_ps(samplePosition + 0.0f*dSample,
                                               samplePosition + 1.0f*dSample,
                                               samplePosition + 2.0f*dSample,
                                               samplePosition + 3.0f*dSample);
                __m128i sampleIndex = _mm_cvttps_epi32(samplePos);
                __m128 frac = _mm_sub_ps(samplePos, _mm_cvtepi32_ps(sampleIndex));                     
                
                __m128 sampleValue0 = _mm_setr_ps(sound->Samples[0][((int32*)&sampleIndex)[0]],
                                                  sound->Samples[0][((int32*)&sampleIndex)[1]],
                                                  sound->Samples[0][((int32*)&sampleIndex)[2]],
                                                  sound->Samples[0][((int32*)&sampleIndex)[3]]);
                __m128 sampleValue1 = _mm_setr_ps(sound->Samples[0][((int32*)&sampleIndex)[0] + 1],
                                                  sound->Samples[0][((int32*)&sampleIndex)[1] + 1],
                                                  sound->Samples[0][((int32*)&sampleIndex)[2] + 1],
                                                  sound->Samples[0][((int32*)&sampleIndex)[3] + 1]);
                                                  
                __m128 sampleValue = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, frac), sampleValue0), _mm_mul_ps(frac, sampleValue1));
#else // nearest-neighbor filtering
                __m128 sampleValue = _mm_setr_ps(sound->Samples[0][RoundReal32ToInt32(samplePosition + 0.0f*dSample)],
                                                 sound->Samples[0][RoundReal32ToInt32(samplePosition + 1.0f*dSample)],
                                                 sound->Samples[0][RoundReal32ToInt32(samplePosition + 2.0f*dSample)],
                                                 sound->Samples[0][RoundReal32ToInt32(samplePosition + 3.0f*dSample)]);
#endif 
                
                // NOTE(Joey): write 4 SIMD wide 
                __m128 d0 = _mm_load_ps((float*)&channel0[0]);
                __m128 d1 = _mm_load_ps((float*)&channel1[0]); 
                
                d0 = _mm_add_ps(d0, _mm_mul_ps(_mm_mul_ps(masterVolume4, volume4_0), sampleValue));
                d1 = _mm_add_ps(d1, _mm_mul_ps(_mm_mul_ps(masterVolume4, volume4_1), sampleValue));
                                
                _mm_store_ps((float*)&channel0[0], d0);
                _mm_store_ps((float*)&channel1[0], d1);
                
                ++channel0;
                ++channel1;
              
                volume4_0 = _mm_add_ps(volume4_0, dVolume4_0);
                volume4_1 = _mm_add_ps(volume4_1, dVolume4_1);
                             
                // samplePosition += 4.0f*dSample;
            }
            
            // playingSound->SamplesPlayed = samplePosition;
            playingSound->SamplesPlayed = endSamplePosition;
            playingSound->CurrentVolume[0] = ((real32*)&volume4_0)[0];
            playingSound->CurrentVolume[1] = ((real32*)&volume4_1)[0];
            
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
            if(playingSound->Loop && playingSound->SamplesPlayed >= (r32)sound->SampleCount)
               playingSound->SamplesPlayed = playingSound->SamplesPlayed - (r32)sound->SampleCount;
          
            
            soundFinished = !playingSound->Loop && (uint32)playingSound->SamplesPlayed >= sound->SampleCount; 
            if(soundFinished)
                dSample = 0.0f;
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