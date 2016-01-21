#include "voidt.h"


internal void GameUpdateAndRender(game_offscreen_buffer *screenBuffer)
{
    int XOffset = 0;
    int YOffset = 0;
    // render
    uint8 *row = (uint8 *)screenBuffer->Memory; 
    for(int Y = 0; Y < screenBuffer->Height; ++Y)
    {
        uint32 *Pixel = (uint32 *)row;
        for(int X = 0; X < screenBuffer->Width; ++X)
        {    
            uint8 Blue  = X + XOffset; // B
            uint8 Green = Y + YOffset; // G
            uint8 Red   = X + YOffset + XOffset;           // R
                        
            *Pixel++ = (Red << 16) | (Green << 8) | Blue;
        }
        row += screenBuffer->Pitch;
    }        
}