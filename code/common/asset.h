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

enum asset_flags {
    // asset type
    ASSET_TYPE_MASK    = 0x00FF,
    ASSET_TYPE_SOUND   = 0x0001,
    ASSET_TYPE_TEXTURE = 0x0002,
    
    // asset state
    ASSET_STATE_MASK     = 0xFF00,
    ASSET_STATE_QUEUED   = 0x0100,
    ASSET_STATE_LOCKED   = 0x0200,
    ASSET_STATE_UNLOADED = 0x0400,
    ASSET_STATE_LOADED   = 0x0800,
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

struct loaded_font
{
    game_font Font;
    char *Name;
};

struct Asset
{
    memory_block *Memory;
    
    char *Name;
    u32   ReferenceCount;
    u32   DataMemorySize;
    
    u16   Flags;
    
    union
    {
        Sound     SoundAsset;
        Texture   TextureAsset;
        game_font FontAsset;
    };    
};

const int MAX_ASSETS = 256;
struct GameAssets
{
    memory_arena *Arena;
    general_purpose_allocater *Memory;
    
    LoadedTexture Textures[MAX_ASSETS];
    LoadedSound   Sounds[MAX_ASSETS];   
    game_font     Font;
    game_font     DebugFont;
    
    // TODO(Joey): use unified asset loading system for easy load|eviction
    // TODO(Joey): transform unified system to hash table
    Asset         Assets[MAX_ASSETS];
    
    u64 MemoryInUse;
    u64 MemoryMax;
    u32 MemorySafetyRegion;
        
    volatile u32 LoadedTextureCount;
    volatile u32 LoadedSoundCount;
};


#endif