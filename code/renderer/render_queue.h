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
#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

struct RenderQueueItem
{
    vector2D Position;  // NOTE(Joey): depth in position, or individual uint32 component?
    uint32   Depth;
    vector2D Basis[2];
    vector2D Size;
    
    Texture *Texture;
    vector4D Color;
};

struct RenderQueue
{   
    uint32 RenderCount;
    uint32 MaxRenderCount;
    RenderQueueItem *RenderItems; 
};

#endif