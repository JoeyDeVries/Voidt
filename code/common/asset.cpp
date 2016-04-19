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

#include <string.h>

inline bool32 StringCompare(char *a, char *b)
{
    return strcmp(a, b) == 0;
}

struct LoadAssetData 
{
    GameAssets *Assets;
    char *FileName;
};

void DoBackgroundTextureLoadWork(platform_work_queue *queue, void* data)
{
    LoadAssetData *assetData = (LoadAssetData*)data;
    
    uint32 currTexturePos = assetData->Assets->LoadedTextureCount;
    uint32 nextTexturePos = _InterlockedCompareExchange((volatile long*)&assetData->Assets->LoadedTextureCount,
                                                currTexturePos + 1, 
                                                currTexturePos);
    if(nextTexturePos == currTexturePos)
    {
        Texture texture = LoadTexture(assetData->Assets->DEBUGPlatformReadEntireFile, assetData->FileName);
                
        LoadedTexture loaded;
        loaded.Asset = texture;
        loaded.Name = assetData->FileName;
        
        assetData->Assets->Textures[nextTexturePos] = loaded;
    }       
}

void DoBackgroundSoundLoadWork(platform_work_queue *queue, void* data)
{
    LoadAssetData *assetData = (LoadAssetData*)data;
     
    uint32 currSoundPos = assetData->Assets->LoadedSoundCount;
    uint32 nextSoundPos = _InterlockedCompareExchange((volatile long*)&assetData->Assets->LoadedSoundCount,
                                                currSoundPos + 1, 
                                                currSoundPos);
    if(nextSoundPos == currSoundPos)
    {
        Sound sound = LoadWAV(assetData->Assets->DEBUGPlatformReadEntireFile, assetData->FileName);
        
        LoadedSound loaded;
        loaded.Asset = sound;
        loaded.Name = assetData->FileName;
        
        assetData->Assets->Sounds[nextSoundPos] = loaded;
    }
}

internal Texture* GetTexture(GameAssets *assets, char *name, bool forceLoad = false)
{
    Texture *texture = 0;
    for(uint32 i = 0; i < assets->LoadedTextureCount; ++i)
    {
        LoadedTexture *asset = assets->Textures + i;
        if(asset->Asset.Texels)
        {
            if(StringCompare(asset->Name, name))
            {
                texture = &asset->Asset;
                break;
            }
        }
    }
    
    // NOTE(Joey): if texture wasn't found, set up background worker to load texture
    if(!texture)
    {           
        if(forceLoad)
        {   // load right now, don't wait for background thread
    
            Texture texturez = LoadTexture(assets->DEBUGPlatformReadEntireFile, name);
            assets->Textures[assets->LoadedTextureCount].Asset = texturez;
            assets->Textures[assets->LoadedTextureCount].Name = name;
            texture = &assets->Textures[assets->LoadedTextureCount].Asset;
            _InterlockedIncrement((volatile long*)&assets->LoadedTextureCount);
        }
        else
        {
            LoadAssetData *data = PushStruct(assets->Arena, LoadAssetData);
            data->Assets = assets;
            data->FileName = name;
            
            // TODO(Joey): make sure to de-allocate memory once task is done (see task_with_memory)
            PlatformAddWorkEntry(assets->WorkQueue, DoBackgroundTextureLoadWork, data);
        }



     
    }
    
    return texture;
}    

internal Sound* GetSound(GameAssets *assets, char *name, bool forceLoad = false)
{
    Sound *sound = 0;
    for(uint32 i = 0; i < assets->LoadedSoundCount; ++i)
    {
        LoadedSound *asset = assets->Sounds + i;
        if(asset->Asset.SampleCount > 0)
        {
            if(StringCompare(asset->Name, name))
            {
                sound = &asset->Asset;
                break;
            }
        }
    }
    
    // NOTE(Joey): if texture wasn't found, set up background worker to load texture
    if(!sound)
    {              
        if(forceLoad)
        {   // load right now, don't wait for background thread
            Sound soundz = LoadWAV(assets->DEBUGPlatformReadEntireFile, name);
            assets->Sounds[assets->LoadedSoundCount].Asset = soundz;
            assets->Sounds[assets->LoadedSoundCount].Name = name;
            // TODO(Joey): make PushString and store const char* memory in arena (as memory addresses
            // of the strings get invalidated while hot-loading DLL.            
            sound = &assets->Sounds[assets->LoadedSoundCount].Asset;
            _InterlockedIncrement((volatile long*)&assets->LoadedSoundCount);
        }
        else
        {
            LoadAssetData *data = PushStruct(assets->Arena, LoadAssetData);
            data->Assets = assets;
            data->FileName = name;
            
            PlatformAddWorkEntry(assets->WorkQueue, DoBackgroundSoundLoadWork, data);      
        }
    }
    
    return sound;
}  

internal void PreFetchTexture(GameAssets *assets, char *name, bool forceLoad = false)
{
    GetTexture(assets, name, forceLoad);
}

internal void PreFetchSound(GameAssets *assets, char *name, bool forceLoad = false)
{
    GetSound(assets, name, forceLoad);
}