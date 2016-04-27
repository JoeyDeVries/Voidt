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

// TODO(Joey): write custom string compare function (remove dependency on c-standard library: string.h)
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
        Texture texture = LoadTexture(PlatformAPI.DEBUGReadEntireFile, assetData->FileName);
                
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
        Sound sound = LoadWAV(PlatformAPI.DEBUGReadEntireFile, assetData->FileName);
        
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
    
            Texture texturez = LoadTexture(PlatformAPI.DEBUGReadEntireFile, name);
            assets->Textures[assets->LoadedTextureCount].Asset = texturez;
            assets->Textures[assets->LoadedTextureCount].Name = PushString(assets->Arena, name);
            texture = &assets->Textures[assets->LoadedTextureCount].Asset;
            _InterlockedIncrement((volatile long*)&assets->LoadedTextureCount);
        }
        else
        {
            LoadAssetData *data = PushStruct(assets->Arena, LoadAssetData);
            data->Assets = assets;
            data->FileName = PushString(assets->Arena, name);
            
            // TODO(Joey): make sure to de-allocate memory once task is done (see task_with_memory)
            PlatformAPI.AddWorkEntry(PlatformAPI.WorkQueueLowPriority, DoBackgroundTextureLoadWork, data);
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
            Sound soundz = LoadWAV(PlatformAPI.DEBUGReadEntireFile, name);
            assets->Sounds[assets->LoadedSoundCount].Asset = soundz;
            assets->Sounds[assets->LoadedSoundCount].Name = PushString(assets->Arena, name);
            // TODO(Joey): make PushString and store const char* memory in arena (as memory addresses
            // of the strings get invalidated while hot-loading DLL.            
            sound = &assets->Sounds[assets->LoadedSoundCount].Asset;
            _InterlockedIncrement((volatile long*)&assets->LoadedSoundCount);
        }
        else
        {
            LoadAssetData *data = PushStruct(assets->Arena, LoadAssetData);
            data->Assets = assets;
            data->FileName = PushString(assets->Arena, name);
            
            PlatformAPI.AddWorkEntry(PlatformAPI.WorkQueueLowPriority, DoBackgroundSoundLoadWork, data);      
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

internal void EvictAssetsAsNecessary(GameAssets *assets)
{
    // NOTE(Joey): check if we use more memory than say -16 MB from the max
    // we use a safety region as a soft cap, s.t. there is always some free 
    // in case new memory gets loaded in 
    while(assets->MemoryInUse > assets->MemoryMax - assets->MemorySafetyRegion)
    {
        /* NOTE(Joey):
        
          To determine which assets are the least used, since this threshold is reached
          use reference counting on the assets. Every time an asset gets loaded increase
          its reference count (32 bit unsigned integer). Then at this stage; sort all
          assets based on this reference count (ascending) and delete the first on the 
          list until the size constraint is reached again. 
          
          This seems like a more efficient (and easier to maintain) approach than a 
          doubly linked list.
          
          Note, be sure to lock assets when doing background work (with an asset load 
          type enum: LOADED|LOCKED|QUEUED etc. Then only evict when assets aren't 
          being used by background threads. Enum of type asset_state.
          
          Then once assets have been cleared; clear ALL reference count values to 0.
          
          When assets are being loaded by multithreaded workers; set state to queued.
          
          Option: instead of keeping a memory buffer and call it once at the end of 
          each frame; call EvictAssetsAsNecessary in the AcquireAssetMemory step, 
          that way there is no need for a buffer; but could partially delay the system?
          Probably wise to check out both options and select the better of them, 
          evicting memory when requested seems the best API design.
          
        */
    }
    
}