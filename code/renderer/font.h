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
#ifndef FONT_H
#define FONT_H

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

struct game_font
{
    stbtt_fontinfo FontInfo;
    
    r32 *HorizontalAdvance;
    r32  LineAdvance;
    
    u32 CodePointCount;
    Texture *CodePoints;
};

#endif