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
#include "texture.cpp"
#include "render_primitives.cpp"
#include "render_queue.cpp"


void RenderQueueItems(RenderQueue *queue, Texture *target, rectangle2Di clipRect)
{
    for(uint32 i = 0; i < queue->RenderCount; ++i)
    {
        RenderQueueItem *item = queue->RenderItems + i;
        
        // NOTE(Joey): define types of render items to allow for custom-tailored rendering
        // code below, including FX (that operate on render target as a whole for instance).
        if(item->Texture)
        {   // NOTE(Joey): render as texture
            RenderTexture_(target, 
                           item->Texture,
                           item->Position,
                           item->Size,
                           item->Basis[0],
                           item->Basis[1],
                           clipRect,
                           item->Color);            
        }
        else
        {   // NOTE(Joey): render as rectangle
            // NOTE(Joey): doesn't support clipping yet; build it or somply disband rectangle rendering?
            RenderRectangle_(target,
                             item->Position,
                             item->Size,
                             item->Color);
        }
    }    
}

struct TiledRenderData 
{
    RenderQueue *Queue;
    Texture     *Target;
    rectangle2Di ClipRect;    
};

void DoTiledRenderWork(platform_work_queue *queue, void* data)
{
    TiledRenderData *tiledData = (TiledRenderData*)data;
    RenderQueueItems(tiledData->Queue, tiledData->Target, tiledData->ClipRect);
}

internal void RenderPass(platform_work_queue *workQueue, RenderQueue *renderQueue, Texture *target)
{
    // NOTE(Joey): sort render items based on item->Depth before rendering
    
    
    // NOTE(Joey): tiled multithreaded rendering.
    const uint8 tileCountX  = 4;
    const uint8 tileCountY  = 4; 
    uint16 tileWidth  = target->Width / tileCountX;
    uint16 tileHeight = target->Height / tileCountY;
    tileWidth = ((tileWidth + 3) / 4) * 4; // memory alignment
    
    // store TiledRenderData structs here to keep them in the stack
    TiledRenderData tiledData[tileCountX * tileCountY];
    uint16 tileCount = 0;
    for(uint8 y = 0; y < tileCountY; ++y)
    {
        for(uint8 x = 0; x < tileCountX; ++x)
        {
            // NOTE(Joey): define clip rectangles to subdivide render work among multiple threads
            rectangle2Di clipRect;
            clipRect.MinX = x*tileWidth;
            clipRect.MaxX = clipRect.MinX + tileWidth;
            clipRect.MinY = y*tileHeight;
            clipRect.MaxY = clipRect.MinY + tileHeight;

            if(x == (tileCountX - 1))
                clipRect.MaxX = target->Width;
            if(y == (tileCountY - 1))
                clipRect.MaxY = target->Height;
            
            TiledRenderData *data = tiledData + tileCount++;
            data->Queue = renderQueue;
            data->Target = target;
            data->ClipRect = clipRect;
            
            if(workQueue)
            {   // NOTE(Joey): we support multithreaded rendering
                PlatformAddWorkEntry(workQueue, DoTiledRenderWork, data);
            }
            else
            {   // NOTE(Joey): else render single-threaded
                RenderQueueItems(renderQueue, target, { 0, 0, (int32)target->Width, (int32)target->Height });
            }
        }
    }
    if(workQueue)
        PlatformCompleteAllWork(workQueue);
   
#if 0
    for(uint32 i = 0; i < queue->RenderCount; ++i)
    {
        RenderQueueItem *item = queue->RenderItems + i;
        
        // NOTE(Joey): define clip rectangles to subdivide render work among multiple threads
        rectangle2Di fullClipRect = { 0, 0, (int32)target->Width, (int32)target->Height };        
        
        // NOTE(Joey): define types of render items to allow for custom-tailored rendering
        // code below, including FX (that operate on render target as a whole for instance).
        if(item->Texture)
        {   // NOTE(Joey): render as texture
            RenderTexture_(target, 
                           item->Texture,
                           item->Position,
                           item->Size,
                           item->Basis[0],
                           item->Basis[1],
                           fullClipRect,
                           item->Color);            
        }
        else
        {   // NOTE(Joey): render as rectangle
            RenderRectangle_(target,
                             item->Position,
                             item->Size,
                             item->Color);
        }
    }    
#endif
}