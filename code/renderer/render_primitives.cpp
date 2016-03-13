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


internal void RenderRectangle_(game_offscreen_buffer* buffer, vector2D min, vector2D max, vector4D color)
{
    
    
};

internal void RenderTexture_(game_offscreen_buffer* buffer, Texture *texture, vector2D position, vector2D scale, vector2D basisX, vector2D basisY, vector4D color)
{
    int32 minX = RoundReal32ToInt32(X);
    int32 minY = RoundReal32ToInt32(Y);
    int32 maxX = minX + scale.x;
    int32 maxY = minY + scale.y;
    
    int32 sourceOffsetX = 0; // fixing clipping
    int32 sourceOffsetY = 0;
    if(minX < 0) 
    {
        sourceOffsetX = -minX;
        minX = 0;
    }
    if(minY < 0)
    {
        sourceOffsetY = -minY;
        minY = 0;
    }
    if(maxX > buffer->Width)
    { 
        maxX = buffer->Width;
    }
    if(maxY > buffer->Height) 
    {
        maxY = buffer->Height;
    }
    
    uint8 *destRow = (uint8*)buffer->Memory + minX * sizeof(uint32) + minY * buffer->Pitch;
    
    for(int y = minY; y < maxY; ++y)
    {
        uint32 *dest = (uint32*)destRow;
        for(int x = minX; x < maxX; ++x)
        {          
            // NOTE(Joey): determine if pixel is in rotated region
            vector2D axisX = scale.X * basisX;
            vector2D axisY = scale.Y * basisY;
            vector2D newPos = { position*axisX + position*axisY }; // transform to new coordinate system
            vector2D d = newPos - position; // from origin of system to new point
            
            // NOTE(Joey): now take dot product of vector d with 4 edges of rotated quad (take perpendicular of axis as edges)
            if(InnerProduct(d, Perpendicular(basisX)) < 0) &&
               InnerProduct(d, Perpendicular(-basisY)) < 0) &&
               InnerProduct(d, Perpendicular(basisX)) < 0) &&
               InnerProduct(d, Perpendicular(-basisY)) < 0))
           {
                // NOTE(Joey): we're inside the rotated quadriliteral, now get UV, sample from texture and render.
                // d*x => |d||x|*cosa; now get d projected on x normalized => (|d||x|cosa)/ |x|^2 => (d*x)/(x*x)
                vector2D UV = { InnerProduct(d, axisX) / InnerProduct(axisX, axisX), 
                                InnerProduct(d, axisY) / InnerProduct(axisY, axisY) };
                uint32 color = TextureSample(texture, UV, true); 
               
               *dest = color;
           }
            
    
            // real32 A = (real32)((*source >> 24) & 0xFF) / 255.0f;
            // A *= alpha;
            // real32 SR = (real32)((*source >> 16) & 0xFF);
            // real32 SG = (real32)((*source >> 8) & 0xFF);
            // real32 SB = (real32)((*source >> 0) & 0xFF);
            
            // real32 DR = (real32)((*dest >> 16) & 0xFF);
            // real32 DG = (real32)((*dest >> 8) & 0xFF);
            // real32 DB = (real32)((*dest >> 0) & 0xFF);
    
            // real32 R = (1.0f - A)*DR + A*SR;
            // real32 G = (1.0f - A)*DG + A*SG;
            // real32 B = (1.0f - A)*DB + A*SB;
            
            // *dest = ((uint32)(R + 0.5) << 16) |
                    // ((uint32)(G + 0.5) << 8) |
                    // ((uint32)(B + 0.5) << 0);
    
            // dest++; source++;
        }          
        destRow += buffer->Pitch;
    }
    
    
}