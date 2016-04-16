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
