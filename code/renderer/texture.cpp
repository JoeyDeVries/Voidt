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

inline vector4D TexelToVector4D(uint32 *texel)
{
    vector4D color = { (real32)((*texel >> 16) & 0xFF), 
                       (real32)((*texel >> 8) & 0xFF),
                       (real32)((*texel >> 0) & 0xFF),
                       (real32)((*texel >> 24) & 0xFF) / 255.0f };     
    return color;
}

// TODO(Joey): work out overall engine structure before implementing details
// TODO(Joey): work out wrapping modes w/ uv coordinates out of bound
internal vector4D TextureSample(Texture *texture, 
                                vector2D uv, 
                                bool32 bilinear = true)
{
    // SEE https://hero.handmadedev.org/videos/game-architecture/day093.html for implementation.    
    /* NOTE(Joey): How texture sampling works:
        
      Get a normalized UV texture coordinate vector; this can be retrieved by
      projecting a point in a quadriliteral onto both its axis and normalizing
      the result. With D being the point between the coordinate system's origin
      and the point in the quadriliteral we get U: D*XAxis / Length(XAxis)^2 
      and similarly for the YAxis. We divide by Length(axis)^2 s.t. we remove
      the axis length component from the dot product result and divide the 
      subsequent result by the length of the axis to normalize its value.
      
      Given the UV texture coordinate vector, transform coordinate to a texel
      with integer clamp and sample the respective texture bitmap. If bilinear
      filtering is enabled surrounding texels are sampled as well and sampled
      based on a weight defined by the floating point's location in the 
      neighbouring texels.

      The color is returned as 4 color bytes.    
    */
    
    // 1. Transform UV data to bitmap space
    // 2. Sample from texture bitmap
    // 3. If bilinear sampling is enabled: similarly sample from neighbouring texels

    // NOTE(Joey): scale UV to texture size
    uv.x = Clamp01(uv.x);
    uv.y = Clamp01(uv.y);
    uv = Hadamard(uv, { (real32)texture->Width - 2, (real32)texture->Height - 2});

    // NOTE(Joey): transform to texture sampling data
    uint32 X = (uint32)uv.x;
    uint32 Y = (uint32)uv.y;
    real32 dX = uv.x - X;
    real32 dY = uv.y - Y;     
    
    Assert(X >= 0 && X <= texture->Width);
    Assert(Y >= 0 && Y <= texture->Height);
    
    // NOTE(Joey): sample from texture
    uint32 *texel00 = (uint32*)((uint8*)texture->Texels + Y*texture->Pitch + X*sizeof(uint32));           
    vector4D color = TexelToVector4D(texel00);       
    
    if(bilinear)
    {   // lerp between multiple samples given dX and dY
        uint32 *texel10 = texel00 + 1;
        uint32 *texel01 = texel00 + (texture->Pitch/sizeof(uint32));
        uint32 *texel11 = texel01 + 1;
        
        vector4D horizontalLerpTop    = Lerp(TexelToVector4D(texel00), TexelToVector4D(texel10), dX);
        vector4D horizontalLerpBottom = Lerp(TexelToVector4D(texel01), TexelToVector4D(texel11), dX);
        color = Lerp(horizontalLerpTop, horizontalLerpBottom, dY);
    }
    
    return color;
}


// .BMP Texture loading
internal Texture LoadTexture(debug_platform_read_entire_file *readEntireFile, 
                             char *fileName)
{
    Texture result = {};
    
    debug_read_file_result readResult = readEntireFile(fileName);    
    if(readResult.ContentSize != 0)
    {
        bitmap_header *header = (bitmap_header *)readResult.Contents;
        uint32 *pixels = (uint32*)((uint8*)readResult.Contents + header->BitmapOffset);
        result.Texels = pixels;
        result.Width  = (uint16)header->Width;
        result.Height = (uint16)header->Height;
        

        //Assert(header->Compression == 3);
        
        // NOTE(Joey): byte order in memory is AA BB GG RR (AA first in lowest memory address), bottom upper_bound
        // CPU reads it in as: RR GG BB AA (first reads AA, then BB)
        // we need AA first so switch AA to the back for each pixel
        // !!!actually: byte order is determined by the header itself with 3 masks for each individual color
        
        uint32 redMask = header->RedMask;
        uint32 greenMask = header->GreenMask;
        uint32 blueMask = header->BlueMask;
        uint32 alphaMask = ~(redMask | greenMask | blueMask);
        
        uint32 redShift = 0;
        uint32 greenShift = 0;
        uint32 blueShift = 0;
        uint32 alphaShift = 0;              
        
        bool32 found = FindLeastSignificantSetBit(&redShift, redMask);
        Assert(found);
        found = FindLeastSignificantSetBit(&greenShift, greenMask);
        Assert(found);
        found = FindLeastSignificantSetBit(&blueShift, blueMask);
        Assert(found);
        found = FindLeastSignificantSetBit(&alphaShift, alphaMask);
        Assert(found);
        
        redShift = 16 - (int32)redShift;
        greenShift = 8 - (int32)greenShift;
        blueShift = 0 - (int32)blueShift;
        alphaShift = 24 - (int32)alphaShift;
        
        result.Pitch = result.Width*sizeof(uint32);
        uint32 *sourceDest = pixels;
        for(uint32 y = 0; y < result.Height; ++y)
        {
            for(uint32 x = 0; x < result.Width; ++x)
            {
                uint32 C = *sourceDest;

                *sourceDest++ = (RotateLeft(C & redMask, redShift) |
                                 RotateLeft(C & greenMask, greenShift) |
                                 RotateLeft(C & blueMask, blueShift) |
                                 RotateLeft(C & alphaMask, alphaShift));
            }            
        }
    }
    return result;
}

internal Texture CreateEmptyTexture(memory_arena *arena, 
                                    uint16 width, 
                                    uint16 height)
{
    Texture texture = {};
    texture.Width = width;
    texture.Height = height;
    
    texture.Pitch = width * sizeof(uint32); // NOTE(Joey): 1 pixel is uint32
    texture.Texels = (uint32*)PushSize_(arena, height*texture.Pitch, 16);
    
    // NOTE(Joey): zero all texels
    ZeroSize(texture.Texels, height*texture.Pitch);
    
    return texture;
}    

// NOTE(Joey): more efficient direct blit scheme compared to rendering a full-screen texture into the 
// buffer with normal render code.
internal void BlitTextureToScreen(game_offscreen_buffer *screenBuffer, Texture *texture)
{
    Assert(texture->Width <= screenBuffer->Width);
    Assert(texture->Height <= screenBuffer->Height);
    
    uint32 *dest = (uint32*)screenBuffer->Memory;
    uint32 *src  = texture->Texels;
    for(uint16 y = 0; y < texture->Height; ++y)
    {
        for(uint16 x = 0; x < texture->Width; ++x)
        {
            *dest = *src;            
            ++dest; ++src;
        }
    }    
}
