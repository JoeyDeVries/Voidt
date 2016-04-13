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


internal void RenderPass(RenderQueue *queue, Texture *target)
{
    // NOTE(Joey): sort render items based on item->Depth before rendering
    
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
    
    queue->RenderCount = 0;
}