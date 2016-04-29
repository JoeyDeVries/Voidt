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


internal void RenderRectangle_(Texture *target, 
                               vector2D position, 
                               vector2D size, 
                               vector4D color)
{
    // NOTE(Joey): correct out-of-bounds
    real32 fMinX = Clamp(0.0f, target->Width,  position.x);
    real32 fMinY = Clamp(0.0f, target->Height, position.y);
    real32 fMaxX = Clamp(0.0f, target->Width,  position.x + size.x);
    real32 fMaxY = Clamp(0.0f, target->Height, position.y + size.y);
    
    // NOTE(Joey): get integer position
    uint32 minX = RoundReal32ToUInt32(fMinX);
    uint32 minY = RoundReal32ToUInt32(fMinY);
    uint32 maxX = RoundReal32ToUInt32(fMaxX);
    uint32 maxY = RoundReal32ToUInt32(fMaxY);
    
    // NOTE(Joey): get memory address of pixel render location in memory;
    uint8 *row = (uint8*)target->Texels + minY*target->Pitch + minX*sizeof(uint32);
    
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
        row += target->Pitch;
    }
};


internal void RenderTexture_(Texture *target, 
                             Texture *texture, 
                             vector2D position, 
                             vector2D size, 
                             vector2D basisX, 
                             vector2D basisY, 
                             rectangle2Di clipRect, 
                             vector4D color)
{
    // TIMING(0): Entire draw call.
    // BeginCPUTiming(0); 
    
     // NOTE(Joey): calculate scaled coordinate basis
    vector2D axisX = size.x * basisX;
    vector2D axisY = size.y * basisY;
    // TODO(Joey): think about whether we want this enforced in RenderTexture_ function or whether
    // this should be controlled purely from the incoming position vector in the game-code.
    position = position - 0.5f*axisX - 0.5f*axisY;
    
    // NOTE(Joey): test min max bounds in given coordinate system
    rectangle2Di fillRect = InvertedInfinityRectangle();
    vector2D testPositions[4] = {position, position + axisX, position + axisX + axisY, position + axisY};
    for(int i= 0; i < ArrayCount(testPositions); ++i)
    {
        vector2D testPos = testPositions[i];
        int floorX = FloorReal32ToInt32(testPos.x);
        int ceilX = CeilReal32ToInt32(testPos.x);
        int floorY = FloorReal32ToInt32(testPos.y);
        int ceilY = CeilReal32ToInt32(testPos.y);

        if(fillRect.MinX > floorX) {fillRect.MinX = floorX;}
        if(fillRect.MinY > floorY) {fillRect.MinY = floorY;}
        if(fillRect.MaxX < ceilX) {fillRect.MaxX = ceilX;}
        if(fillRect.MaxY < ceilY) {fillRect.MaxY = ceilY;}
    }   
        
    fillRect = Intersect(clipRect, fillRect);
    if(HasArea(fillRect))
    {              
        // NOTE(Joey): align masks to 4-byte/16-pixel boundary
        __m128i startClipMask = _mm_set1_epi8(-1);
        __m128i endClipMask = _mm_set1_epi8(-1);

        __m128i startClipMasks[] =
        {
            _mm_slli_si128(startClipMask, 0*4),
            _mm_slli_si128(startClipMask, 1*4),
            _mm_slli_si128(startClipMask, 2*4),
            _mm_slli_si128(startClipMask, 3*4),            
        };

        __m128i endClipMasks[] =
        {
            _mm_srli_si128(endClipMask, 0*4),
            _mm_srli_si128(endClipMask, 3*4),
            _mm_srli_si128(endClipMask, 2*4),
            _mm_srli_si128(endClipMask, 1*4),            
        };
        
        if(fillRect.MinX & 3)
        {
            startClipMask = startClipMasks[fillRect.MinX & 3];
            fillRect.MinX = fillRect.MinX & ~3;
        }

        if(fillRect.MaxX & 3)
        {
            endClipMask = endClipMasks[fillRect.MaxX & 3];
            fillRect.MaxX = (fillRect.MaxX & ~3) + 4;
        }        
        
        // SIMD        
        #define mmSquare(a) _mm_mul_ps(a, a)
        #define M(a, i) ((float*)&(a))[i]
        #define Mi(a, i) ((uint32*)&(a))[i]        
        // SIMD globals
        const __m128 zero = _mm_set1_ps(0.0f);
        const __m128 half = _mm_set1_ps(0.5f);
        const __m128 one = _mm_set1_ps(1.0f);
        const __m128 four = _mm_set1_ps(4.0f);
        const __m128i maskFF = _mm_set1_epi32(0xFF);
        const __m128i maskFFFF = _mm_set1_epi32(0xFFFF);
        const __m128i maskFF00FF = _mm_set1_epi32(0x00FF00FF);
        
        const real32 inv255 = 1.0f / 255.0f;
        const __m128 inv255_4x = _mm_set1_ps(inv255);
        
        const __m128 colorr_4x = _mm_set1_ps(color.r);
        const __m128 colorg_4x = _mm_set1_ps(color.g);
        const __m128 colorb_4x = _mm_set1_ps(color.b);
        const __m128 colora_4x = _mm_set1_ps(color.a);
        const __m128 maxColorValue = _mm_set1_ps(255.0f);
            
        const __m128 mAxisXx   = _mm_set1_ps(axisX.x);
        const __m128 mAxisXy   = _mm_set1_ps(axisX.y);
        const __m128 mAxisYx   = _mm_set1_ps(axisY.x);
        const __m128 mAxisYy   = _mm_set1_ps(axisY.y);
        const __m128 positionX = _mm_set1_ps(position.x);
        const __m128 positionY = _mm_set1_ps(position.y);    

        const __m128 invSquareDotAxisX =  _mm_div_ps(one, _mm_add_ps(mmSquare(mAxisXx), mmSquare(mAxisXy))); 
        const __m128 invSquareDotAxisY =  _mm_div_ps(one, _mm_add_ps(mmSquare(mAxisYx), mmSquare(mAxisYy))); 
        const __m128 texWidthM2 = _mm_set1_ps((real32)texture->Width - 2);
        const __m128 texHeightM2 = _mm_set1_ps((real32)texture->Height - 2);
        const __m128i texturePitch_4x = _mm_set1_epi32(texture->Pitch);

        
        int minX = fillRect.MinX;
        int minY = fillRect.MinY;
        int maxX = fillRect.MaxX;
        int maxY = fillRect.MaxY;
                
        uint32 destPitch = target->Width*sizeof(uint32);
        uint8 *destRow = (uint8*)target->Texels + minY*destPitch + minX*sizeof(uint32);
        
        for (int32 y = minY; y < maxY; ++y)
        {
            uint32 *dest = (uint32*)destRow;
            __m128i clipMask = startClipMask;

            __m128 pixelPosY = _mm_set1_ps((real32)y);
            __m128 dY = _mm_sub_ps(pixelPosY, positionY);
            __m128 dYAxisXy = _mm_mul_ps(dY, mAxisXy);
            __m128 dYAxisYy = _mm_mul_ps(dY, mAxisYy);
  
            __m128 pixelPosX = _mm_set_ps((real32)(minX + 3),
                                          (real32)(minX + 2), 
                                          (real32)(minX + 1), 
                                          (real32)(minX + 0));
            __m128 dX = _mm_sub_ps(pixelPosX, positionX);
                               
            for (int32 x = minX; x < maxX; x += 4)
            {
                // TIMING(1): Per pixel CPU timing
                // BeginCPUTiming(1);
                TIMING_BLOCK(4);

                // NOTE(Joey): pre-fetch destination memory at start
                __m128i originalDest = _mm_loadu_si128((__m128i *)dest);

                __m128 uNominator = _mm_add_ps(_mm_mul_ps(dX, mAxisXx), dYAxisXy);
                __m128 U = _mm_mul_ps(invSquareDotAxisX, uNominator);
                __m128 vNominator = _mm_add_ps(_mm_mul_ps(dX, mAxisYx), dYAxisYy);
                __m128 V = _mm_mul_ps(invSquareDotAxisY, vNominator);                  

                // NOTE(Joey): determine from UV whether we write pixels (none if out of range [0, 1])
                __m128i writeMask = _mm_castps_si128(_mm_and_ps(_mm_and_ps(_mm_cmpge_ps(U, zero),
                                                     _mm_cmple_ps(U, one)),
                                                     _mm_and_ps(_mm_cmpge_ps(V, zero),
                                                     _mm_cmple_ps(V, one))));
                writeMask = _mm_and_si128(writeMask, clipMask);

                // NOTE(Joey): after determining write mask, clamp UV and fetch texels
                U = _mm_min_ps(_mm_max_ps(U, zero), one);
                V = _mm_min_ps(_mm_max_ps(V, zero), one);
                
                // NOTE(Joey): Bias texture coordinates to start on the boundary between 
                // 0,0 and 1,1 pixels.
                __m128 tX = _mm_add_ps(_mm_mul_ps(U, texWidthM2), half);
                __m128 tY = _mm_add_ps(_mm_mul_ps(V, texHeightM2), half);
                
                __m128i fetchX_4x = _mm_cvttps_epi32(tX);
                __m128i fetchY_4x = _mm_cvttps_epi32(tY);
                __m128 fX = _mm_sub_ps(tX, _mm_cvtepi32_ps(fetchX_4x));
                __m128 fY = _mm_sub_ps(tY, _mm_cvtepi32_ps(fetchY_4x));

                fetchX_4x = _mm_slli_epi32(fetchX_4x, 2);
                fetchY_4x = _mm_or_si128(_mm_mullo_epi16(fetchY_4x, texturePitch_4x),
                    _mm_slli_epi32(_mm_mulhi_epi16(fetchY_4x, texturePitch_4x), 16));
                __m128i fetch_4x = _mm_add_epi32(fetchX_4x, fetchY_4x);

                // NOTE(Joey): fetch 4 samples for bilinear blend
                int32 fetch0 = Mi(fetch_4x, 0);
                int32 fetch1 = Mi(fetch_4x, 1);
                int32 fetch2 = Mi(fetch_4x, 2);
                int32 fetch3 = Mi(fetch_4x, 3);

                uint8 *texelPtr0 = ((uint8 *)texture->Texels) + fetch0;
                uint8 *texelPtr1 = ((uint8 *)texture->Texels) + fetch1;
                uint8 *texelPtr2 = ((uint8 *)texture->Texels) + fetch2;
                uint8 *texelPtr3 = ((uint8 *)texture->Texels) + fetch3;

                __m128i sampleA = _mm_setr_epi32(*(uint32 *)(texelPtr0),
                                                 *(uint32 *)(texelPtr1),
                                                 *(uint32 *)(texelPtr2),
                                                 *(uint32 *)(texelPtr3));

                __m128i sampleB = _mm_setr_epi32(*(uint32 *)(texelPtr0 + sizeof(uint32)),
                                                 *(uint32 *)(texelPtr1 + sizeof(uint32)),
                                                 *(uint32 *)(texelPtr2 + sizeof(uint32)),
                                                 *(uint32 *)(texelPtr3 + sizeof(uint32)));

                __m128i sampleC = _mm_setr_epi32(*(uint32 *)(texelPtr0 + texture->Pitch),
                                                 *(uint32 *)(texelPtr1 + texture->Pitch),
                                                 *(uint32 *)(texelPtr2 + texture->Pitch),
                                                 *(uint32 *)(texelPtr3 + texture->Pitch));

                __m128i sampleD = _mm_setr_epi32(*(uint32 *)(texelPtr0 + texture->Pitch + sizeof(uint32)),
                                                 *(uint32 *)(texelPtr1 + texture->Pitch + sizeof(uint32)),
                                                 *(uint32 *)(texelPtr2 + texture->Pitch + sizeof(uint32)),
                                                 *(uint32 *)(texelPtr3 + texture->Pitch + sizeof(uint32)));

                // NOTE(Joey): unpack bilinear samples
                __m128i texelArb = _mm_and_si128(sampleA, maskFF00FF);
                __m128i texelAag = _mm_and_si128(_mm_srli_epi32(sampleA, 8), maskFF00FF);
                __m128 texelAa = _mm_cvtepi32_ps(_mm_srli_epi32(texelAag, 16));

                __m128i texelBrb = _mm_and_si128(sampleB, maskFF00FF);
                __m128i texelBag = _mm_and_si128(_mm_srli_epi32(sampleB, 8), maskFF00FF);
                __m128 texelBa = _mm_cvtepi32_ps(_mm_srli_epi32(texelBag, 16));

                __m128i texelCrb = _mm_and_si128(sampleC, maskFF00FF);
                __m128i texelCag = _mm_and_si128(_mm_srli_epi32(sampleC, 8), maskFF00FF);
                __m128 texelCa = _mm_cvtepi32_ps(_mm_srli_epi32(texelCag, 16));

                __m128i texelDrb = _mm_and_si128(sampleD, maskFF00FF);
                __m128i texelDag = _mm_and_si128(_mm_srli_epi32(sampleD, 8), maskFF00FF);
                __m128 texelDa = _mm_cvtepi32_ps(_mm_srli_epi32(texelDag, 16));

                // NOTE(Joey): Load destination
                __m128 destb = _mm_cvtepi32_ps(_mm_and_si128(originalDest, maskFF));
                __m128 destg = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(originalDest, 8), maskFF));
                __m128 destr = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(originalDest, 16), maskFF));
                __m128 desta = _mm_cvtepi32_ps(_mm_and_si128(_mm_srli_epi32(originalDest, 24), maskFF));

                // NOTE(Joey): extract individual texel components and cvt. to real.
                __m128 texelAr = _mm_cvtepi32_ps(_mm_srli_epi32(texelArb, 16));
                __m128 texelAg = _mm_cvtepi32_ps(_mm_and_si128(texelAag, maskFFFF));
                __m128 texelAb = _mm_cvtepi32_ps(_mm_and_si128(texelArb, maskFFFF));

                __m128 texelBr = _mm_cvtepi32_ps(_mm_srli_epi32(texelBrb, 16));
                __m128 texelBg = _mm_cvtepi32_ps(_mm_and_si128(texelBag, maskFFFF));
                __m128 texelBb = _mm_cvtepi32_ps(_mm_and_si128(texelBrb, maskFFFF));

                __m128 texelCr = _mm_cvtepi32_ps(_mm_srli_epi32(texelCrb, 16));
                __m128 texelCg = _mm_cvtepi32_ps(_mm_and_si128(texelCag, maskFFFF));
                __m128 texelCb = _mm_cvtepi32_ps(_mm_and_si128(texelCrb, maskFFFF));

                __m128 texelDr = _mm_cvtepi32_ps(_mm_srli_epi32(texelDrb, 16));
                __m128 texelDg = _mm_cvtepi32_ps(_mm_and_si128(texelDag, maskFFFF));
                __m128 texelDb = _mm_cvtepi32_ps(_mm_and_si128(texelDrb, maskFFFF));
                
                // NOTE(Joey): bilinear texture filtering
                __m128 ifX = _mm_sub_ps(one, fX);
                __m128 ifY = _mm_sub_ps(one, fY);
            
                __m128 l0 = _mm_mul_ps(ifY, ifX);
                __m128 l1 = _mm_mul_ps(ifY, fX);
                __m128 l2 = _mm_mul_ps(fY, ifX);
                __m128 l3 = _mm_mul_ps(fY, fX);

                __m128 texelr = _mm_add_ps(_mm_add_ps(_mm_mul_ps(l0, texelAr), _mm_mul_ps(l1, texelBr)),
                                           _mm_add_ps(_mm_mul_ps(l2, texelCr), _mm_mul_ps(l3, texelDr)));
                __m128 texelg = _mm_add_ps(_mm_add_ps(_mm_mul_ps(l0, texelAg), _mm_mul_ps(l1, texelBg)),
                                           _mm_add_ps(_mm_mul_ps(l2, texelCg), _mm_mul_ps(l3, texelDg)));
                __m128 texelb = _mm_add_ps(_mm_add_ps(_mm_mul_ps(l0, texelAb), _mm_mul_ps(l1, texelBb)),
                                           _mm_add_ps(_mm_mul_ps(l2, texelCb), _mm_mul_ps(l3, texelDb)));
                __m128 texela = _mm_add_ps(_mm_add_ps(_mm_mul_ps(l0, texelAa), _mm_mul_ps(l1, texelBa)),
                                           _mm_add_ps(_mm_mul_ps(l2, texelCa), _mm_mul_ps(l3, texelDa)));

                // NOTE(Joey): Modulate by color
                texelr = _mm_mul_ps(texelr, colorr_4x);
                texelg = _mm_mul_ps(texelg, colorg_4x);
                texelb = _mm_mul_ps(texelb, colorb_4x);
                texela = _mm_mul_ps(texela, colora_4x);
                
                texelr = _mm_min_ps(_mm_max_ps(texelr, zero), maxColorValue);
                texelg = _mm_min_ps(_mm_max_ps(texelg, zero), maxColorValue);
                texelb = _mm_min_ps(_mm_max_ps(texelb, zero), maxColorValue);

                // NOTE(Joey): destination/framebuffer blend
                __m128 mulAlpha  = _mm_mul_ps(inv255_4x, texela);
                __m128 invTexelA = _mm_sub_ps(one, mulAlpha);
                __m128 blendedr = _mm_add_ps(_mm_mul_ps(invTexelA, destr), _mm_mul_ps(mulAlpha, texelr));
                __m128 blendedg = _mm_add_ps(_mm_mul_ps(invTexelA, destg), _mm_mul_ps(mulAlpha, texelg));
                __m128 blendedb = _mm_add_ps(_mm_mul_ps(invTexelA, destb), _mm_mul_ps(mulAlpha, texelb));
                __m128 blendeda = _mm_add_ps(_mm_mul_ps(invTexelA, desta), _mm_mul_ps(mulAlpha, texela));
                
                    
                // NOTE(Joey): write back to framebuffer memory
                __m128i intr = _mm_cvtps_epi32(blendedr);
                __m128i intg = _mm_cvtps_epi32(blendedg);
                __m128i intb = _mm_cvtps_epi32(blendedb);
                __m128i inta = _mm_cvtps_epi32(blendeda);
                // NOTE(Joey): pack to windows destination RGBA format
                __m128i sr = _mm_slli_epi32(intr, 16);
                __m128i sg = _mm_slli_epi32(intg, 8);
                __m128i sb = intb;
                __m128i sa = _mm_slli_epi32(inta, 24);
                __m128i out = _mm_or_si128(_mm_or_si128(sr, sg), _mm_or_si128(sb, sa));
                
                // NOTE(Joey): only write to memory where mask bits are set
                __m128i maskedOut = _mm_or_si128(_mm_and_si128(writeMask, out),
                                                 _mm_andnot_si128(writeMask, originalDest));
                _mm_storeu_si128((__m128i *)dest, maskedOut);
 
                dest += 4; 
                dX = _mm_add_ps(dX, four);
                clipMask = _mm_set1_epi8(-1);
                
                if((x + 8) < maxX)
                    clipMask = _mm_set1_epi8(-1);
                else
                    clipMask = endClipMask;

                // EndCPUTiming(1, 4);
            }
            destRow += destPitch;
        }
    }
    // EndCPUTiming(0);
}

///////////////////////////////
//      Render Utility       //
///////////////////////////////
internal void RenderTexture(Texture *target, 
                            Texture *texture, 
                            vector2D position, 
                            vector2D size, 
                            rectangle2Di clipRect, 
                            vector4D color = { 1.0f, 1.0f, 1.0f, 1.0f })
{
    RenderTexture_(target, texture, position, size, { 1.0f, 0.0f }, { 0.0f, 1.0f }, clipRect,  color);
}
