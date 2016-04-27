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

internal stbtt_fontinfo LoadTrueTypeFont(char *filename)
{
    debug_read_file_result readResult = PlatformAPI.DEBUGReadEntireFile(filename);
    
    stbtt_fontinfo font;
    stbtt_InitFont(&font, (u8*)readResult.Contents, stbtt_GetFontOffsetForIndex((u8*)readResult.Contents, 0));

    return font;        
}

internal Texture LoadCharacterGlyph(memory_arena *arena, stbtt_fontinfo font, u32 codePoint, r32 scale = 32.0f)
{
    Texture texture = {};
    
    int width, height, xoffset, yoffset;
    u8 *monoBitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, scale), 
                                              codePoint, &width, &height, &xoffset, &yoffset);
                                 
    texture = CreateEmptyTexture(arena, (u16)width, (u16)height);
    
    u8 *source = monoBitmap;
    u8 *destRow = (u8*)texture.Texels + (texture.Height-1)*texture.Pitch; // go bottom-up
    for(u32 y = 0; y < texture.Height; ++y)
    {
        u32 *dest = (u32*)destRow;
        for(u32 x = 0; x < texture.Width; ++x)
        {
            u8 alpha = *source++;
            u8 color = 0xFF;
            *dest++ = (alpha << 24) |
                      (color << 16) |
                      (color <<  8) |
                      (color <<  0);            
        }
        
        destRow -= texture.Pitch;
    }
    
    stbtt_FreeBitmap(monoBitmap, 0);
    
    return texture;
}