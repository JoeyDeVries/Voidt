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
#ifndef ASSET_H
#define ASSET_H

enum asset_type {
    ASSET_TYPE_TEXTURE,
    ASSET_TYPE_SOUND,
};

struct LoadedTexture
{
    Texture Asset;
    char   *Name;
};

struct LoadedSound
{
    Sound Asset;
    char *Name;    
};

const int MAX_ASSETS = 256;
struct GameAssets
{
    memory_arena *Arena;
    
    // TODO(Joey): transform static list to hash table (more efficient|flexible scheme)
    LoadedTexture Textures[MAX_ASSETS];
    LoadedSound   Sounds[MAX_ASSETS];   
        
    volatile uint32 LoadedTextureCount;
    volatile uint32 LoadedSoundCount;
};


#endif