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

internal game_font* LoadTrueTypeFont(memory_arena *arena, char *filename)
{
    debug_read_file_result readResult = PlatformAPI.DEBUGReadEntireFile(filename);
    
    game_font *font = PushStruct(arena, game_font);
    stbtt_InitFont(&font->FontInfo, (u8*)readResult.Contents, stbtt_GetFontOffsetForIndex((u8*)readResult.Contents, 0));

    font->LineAdvance = 0.0f;
    font->CodePointCount = '~' - '!'; // NOTE(Joey): hardcoded for now as we work in a limited range.
    
    font->CodePoints = (Texture*)PushSize_(arena, font->CodePointCount*sizeof(Texture));
    font->HorizontalAdvance = (r32*)PushSize_(arena, font->CodePointCount*font->CodePointCount*sizeof(r32)); // NOTE(Joey): kerning
    
    return font;        
}

internal Texture LoadCharacterGlyph(memory_arena *arena, game_font *font, u32 codePoint, r32 scale = 32.0f)
{
    // TODO(Joey): get pointer to proper Texture of Font from CodePoint (probably do a codePoint - '!' to get array index)
    // then set Texture struct items and do a PushSize on memory of texture for glyph!
    Texture texture = {};
    
    int width, height, xoffset, yoffset;
    u8 *monoBitmap = stbtt_GetCodepointBitmap(&font->FontInfo, 0, stbtt_ScaleForPixelHeight(&font->FontInfo, scale), 
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

/* TODO(Joey):
  
  Write text functions here: pass in array of characters, read out their 
  codepoints from ASCII, extract glyph information from font struct and
  render the bitmaps accordingly. 
  
  The font struct should have an array of Glyphs where each Glyph holds
  their relative offset data OR the font has a large 2D array of kerning
  pairs that describe the relative offset to neighboring chars. Extract
  this data from the STB_TRUETYPE header. The font struct should have 
  contain a vertical advance offset.

*/  