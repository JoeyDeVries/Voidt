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


internal void RenderRectangle_(game_offscreen_buffer* buffer, vector2D position, vector2D size, vector4D color)
{
    // NOTE(Joey): correct out-of-bounds
    real32 fMinX = Clamp(0.0f, buffer->Width,  position.x);
    real32 fMinY = Clamp(0.0f, buffer->Height, position.y);
    real32 fMaxX = Clamp(0.0f, buffer->Width,  position.x + size.x);
    real32 fMaxY = Clamp(0.0f, buffer->Height, position.y + size.y);
    
    // NOTE(Joey): get integer position
    uint32 minX = RoundReal32ToUInt32(fMinX);
    uint32 minY = RoundReal32ToUInt32(fMinY);
    uint32 maxX = RoundReal32ToUInt32(fMaxX);
    uint32 maxY = RoundReal32ToUInt32(fMaxY);
    
    // NOTE(Joey): get memory address of pixel render location in memory
    uint32 pitch = buffer->Width * sizeof(uint32);
    uint8 *row = (uint8*)buffer->Memory + minY*pitch + minX*sizeof(uint32);
    
    // NOTE(Joey): convert color from floating point to int:0-255
    // NOTE(Joey): we ignore alpha (for now)
    // NOTE(Joey): BIT PATTERN: 0x AA RR GG BB
    uint32 btColor = (RoundReal32ToUInt32(color.r*255.0f) << 16) |
                     (RoundReal32ToUInt32(color.g*255.0f) << 8)  |
                     (RoundReal32ToUInt32(color.b*255.0f) << 0);
    
    // NOTE(Joey): walk over memory in position bounds and fill buffer
    for(uint32 y = minY; y < maxY; ++y)
    {
        uint32 *pixel = (uint32*)row;
        for(uint32 x = minX; x < maxX; ++x)
        {            
            *pixel++ = btColor;
        }
        row += pitch;
    }
};


internal void RenderTexture_(game_offscreen_buffer* buffer, Texture *texture, vector2D position, vector2D size, vector2D basisX, vector2D basisY, vector4D color)
{
     // NOTE(Joey): calculate scaled coordinate basis
    vector2D axisX = size.x * basisX;
    vector2D axisY = size.y * basisY;
    // TODO(Joey): think about whether we want this enforced in RenderTexture_ function or whether
    // this should be controlled purely from the incoming position vector in the game-code.
    position = position - 0.5f*axisX - 0.5f*axisY;
    
    // NOTE(Joey): test min max bounds in given coordinate system
    int32 minX = buffer->Width - 1;
    int32 minY = buffer->Height - 1;
    int32 maxX = 0;
    int32 maxY = 0;
    vector2D testPositions[4] = {position, position + axisX, position + axisX + axisY, position + axisY};
    for(int i= 0; i < ArrayCount(testPositions); ++i)
    {
        vector2D testPos = testPositions[i];
        int floorX = FloorReal32ToInt32(testPos.x);
        int ceilX = CeilReal32ToInt32(testPos.x);
        int floorY = FloorReal32ToInt32(testPos.y);
        int ceilY = CeilReal32ToInt32(testPos.y);

        if(minX > floorX) {minX = floorX;}
        if(minY > floorY) {minY = floorY;}
        if(maxX < ceilX) {maxX = ceilX;}
        if(maxY < ceilY) {maxY = ceilY;}
    }   
    
    if(minX < 0) {minX = 0;}
    if(minY < 0) {minY = 0;}
    if(maxX > buffer->Width - 1 ) { maxX = buffer->Width - 1; }
    if(maxY > buffer->Height - 1) { maxY = buffer->Width - 1; }    
    

    uint32 destPitch = buffer->Width*sizeof(uint32);
    uint8 *destRow = (uint8*)buffer->Memory + minY*destPitch + minX*sizeof(uint32);
    
    uint8 *sourceRow = (uint8*)texture->Texels; // NOTE(Joey): bottom-up
    // sourceRow += -sourceOffsetY*texture->Pitch + sourceOffsetX*sizeof(uint32); // NOTE(Joey): offset source access by however much we clipped 
    
    
    for(int32 y = minY; y < maxY; ++y)
    {
        uint32 *dest = (uint32*)destRow;
        uint32 *source = (uint32*)sourceRow;
        for(int32 x = minX; x < maxX; ++x)
        {          
            if(x == 300 && y == 300)
                int asd = 5;
            
            // NOTE(Joey): determine if pixel is in rotated region
            vector2D pixelPos = { (real32)x, (real32)y };
            vector2D d = pixelPos - position; // from origin of system to new point
            
            // NOTE(Joey): now take dot product of vector d with 4 edges of rotated quad (take perpendicular of axis as edges)
            // TODO(Joey): try and compare with own equation
            if(InnerProduct(d,                 -Perpendicular(axisX))  < 0 &&
               InnerProduct(d - axisX ,        -Perpendicular(axisY))  < 0 &&
               InnerProduct(d - axisX - axisY,  Perpendicular(axisX))  < 0 &&
               InnerProduct(d - axisY,          Perpendicular(axisY))  < 0)
           {
                // NOTE(Joey): we're inside the rotated quadriliteral, now get UV, sample from texture and render.
                // d*x => |d||x|*cosa; now get d projected on x normalized => (|d||x|cosa)/ |x|^2 => (d*x)/(x*x)
                vector2D UV = { InnerProduct(d, axisX) / InnerProduct(axisX, axisX), 
                                InnerProduct(d, axisY) / InnerProduct(axisY, axisY) };
                vector4D sampled = TextureSample(texture, UV, true); 
                
                real32 A = sampled.a;
                real32 DR = (real32)((*dest >> 16) & 0xFF);
                real32 DG = (real32)((*dest >> 8) & 0xFF);
                real32 DB = (real32)((*dest >> 0) & 0xFF);
        
                real32 R = (1.0f - A)*DR + A*sampled.r;
                real32 G = (1.0f - A)*DG + A*sampled.g;
                real32 B = (1.0f - A)*DB + A*sampled.b;
                            
                *dest = ((uint32)(R + 0.5) << 16) |
                        ((uint32)(G + 0.5) << 8) |
                        ((uint32)(B + 0.5) << 0);

                // *dest = 0xFFFFFFFF;
        
             
           }
           
           // *dest = 0xFFFFFFFF;
        
                // dest++; source++;

            
    
            // real32 A = (real32)((*source >> 24) & 0xFF) / 255.0f;
            // A *= color.a;
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
            dest++; source++;
        }          
        destRow += destPitch;
        sourceRow += texture->Pitch;
    }       
}

///////////////////////////////
//      Render Utility       //
///////////////////////////////
internal void RenderTexture(game_offscreen_buffer* buffer, Texture *texture, vector2D position, vector2D size, vector4D color = { 1.0f, 1.0f, 1.0f, 1.0f })
{
    RenderTexture_(buffer, texture, position, size, { 1.0f, 0.0f }, { 0.0f, 1.0f }, color);
}
