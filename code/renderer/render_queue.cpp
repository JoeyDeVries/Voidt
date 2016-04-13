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


internal RenderQueue *CreateRenderQueue(memory_arena *arena, uint32 maxRenderCount)
{
    RenderQueue *queue = PushStruct(arena, RenderQueue);       
    queue->RenderCount = 0;
    queue->MaxRenderCount = maxRenderCount;
    queue->RenderItems = PushArray(arena, maxRenderCount, RenderQueueItem);    
    
    return queue;
}

///////////////////////////////////////////////
//      RECTANGLE RENDER OVERLOADS
///////////////////////////////////////////////
internal void PushRectangle(RenderQueue *queue, 
                            vector2D position, 
                            uint32 depth,
                            vector2D size,
                            vector4D color = { 1.0f, 1.0f, 1.0f, 1.0f })
{
    Assert(queue->RenderCount + 1 < queue->MaxRenderCount);
    RenderQueueItem *renderItem = queue->RenderItems + queue->RenderCount;
    
    renderItem->Position = position;
    renderItem->Depth    = depth;
    renderItem->Basis[0] = vector2D { 1.0f, 0.0f };
    renderItem->Basis[1] = vector2D { 0.0f, 1.0f };
    renderItem->Size     = size;
    renderItem->Texture  = 0;
    renderItem->Color    = color;
    
    queue->RenderCount += 1;
}

///////////////////////////////////////////////
//      TEXTURED RECTANGLE RENDER OVERLOADS
///////////////////////////////////////////////
internal void PushTexture(RenderQueue *queue, 
                          Texture *texture, 
                          vector2D position, 
                          uint32 depth,
                          vector2D size, 
                          vector2D basisX, 
                          vector2D basisY, 
                          vector4D color)
{
    Assert(queue->RenderCount + 1 < queue->MaxRenderCount);
    RenderQueueItem *renderItem = queue->RenderItems + queue->RenderCount;
    
    renderItem->Position = position;
    renderItem->Depth    = depth;
    renderItem->Basis[0] = basisX;
    renderItem->Basis[1] = basisY;
    renderItem->Size     = size;
    renderItem->Texture  = texture;
    renderItem->Color    = color;
    
    queue->RenderCount += 1;
}

internal void PushTexture(RenderQueue *queue, 
                          Texture *texture, 
                          vector2D position, 
                          uint32 depth,
                          vector2D size, 
                          vector4D color = { 1.0f, 1.0f, 1.0f, 1.0f })
{
    vector2D basisX = { 1.0f, 0.0f };
    vector2D basisY = { 0.0f, 1.0f };
    PushTexture(queue, texture, position, depth, size, basisX, basisY, color); 
}

