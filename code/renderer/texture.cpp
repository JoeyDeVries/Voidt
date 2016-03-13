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

// TODO(Joey): work out overall engine structure before implementing details
// TODO(Joey): work out wrapping modes w/ uv coordinates out of bound
internal uint32 TextureSample(Texture *texture, vector2D uv, bool32 bilinear = true)
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
    
    // 1. Cast UV to int
    // 2. Sample from texture bitmap
    // 3. Also sample from neighbouring texels and do bilinear filtering if enabled
    // 4. Convert color data to ARGB uint32 and return
    
    return 0;
}




// .BMP Texture loading
internal Texture LoadTexture(thread_context *thread, debug_platform_read_entire_file *readEntireFile, char *fileName)
{
    Texture result = {};
    
    debug_read_file_result readResult = readEntireFile(thread, fileName);    
    if(readResult.ContentSize != 0)
    {
        bitmap_header *header = (bitmap_header *)readResult.Contents;
        uint32 *pixels = (uint32*)((uint8*)readResult.Contents + header->BitmapOffset);
        result.Texels = pixels;
        result.Width= header->Width;
        result.Height = header->Height;
        
        Assert(header->Compression == 3);
        
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
        
        uint32 *sourceDest = pixels;
        for(uint32 y = 0; y < result.Height; ++y)
        {
            for(uint32 x = 0; x < result.Width; ++x)
            {
                uint32 C = *sourceDest;
#if 0                
                *sourceDest++ = (((C >> alphaShift) & 0xFF) << 24) | 
                                (((C >> redShift  ) & 0xFF) << 16) | 
                                (((C >> greenShift) & 0xFF) << 8)  | 
                                (((C >> blueShift ) & 0xFF) << 0); 
#else
                *sourceDest++ = (RotateLeft(C & redMask, redShift) |
                                 RotateLeft(C & greenMask, greenShift) |
                                 RotateLeft(C & blueMask, blueShift) |
                                 RotateLeft(C & alphaMask, alphaShift));
#endif
            }            
        }
    }
    return result;
}
