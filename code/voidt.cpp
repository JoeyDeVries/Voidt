#include "voidt.h"

internal void GameRender(game_offscreen_buffer *screenBuffer, int xOffset, int yOffset)
{
    // int XOffset = 0;
    // int YOffset = 0;
    // render
    uint8 *row = (uint8 *)screenBuffer->Memory; 
    for(int Y = 0; Y < screenBuffer->Height; ++Y)
    {
        uint32 *Pixel = (uint32 *)row;
        for(int X = 0; X < screenBuffer->Width; ++X)
        {    
            uint8 Blue  = X + xOffset; // B
            uint8 Green = Y + yOffset; // G
            uint8 Red   = X + yOffset + xOffset;           // R
                        
            *Pixel++ = (Red << 16) | (Green << 8) | Blue;
        }
        row += screenBuffer->Pitch;
    }            
}



internal void GameOutputSound(game_sound_output_buffer *soundBuffer, int toneHz)
{
    local_persist real64 tSine;
    int16 toneVolume = 3000;
    // int16 toneHz = 256;
    int16 wavePeriod = soundBuffer->SamplesPerSecond / toneHz;
    
    int16 *sampleOut = soundBuffer->Samples;
    for(int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
    {
        real32 sineValue = sinf(tSine);
        int16 sampleValue = (int16)(sineValue * toneVolume);
        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;
        
        tSine += 2.0f * Pi32 * 1.0f / wavePeriod;
    }        
}

internal void GameUpdateAndRender(game_memory *memory,
                                  game_input *input,
                                  game_offscreen_buffer *screenBuffer, 
                                  game_sound_output_buffer *soundBuffer)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;
    Assert(sizeof(game_state) <= memory->PermanentStorageSize); 
    
    if(!memory->IsInitialized)
    {        
        char *fileName = __FILE__;
        debug_read_file_result file = DEBUGPlatformReadEntireFile(fileName);
        
        if(file.Contents)
        {
            DEBUGPlatformWriteEntireFile("W:/data/test.out", file.ContentSize, file.Contents);
            DEBUGPlatformFreeFileMemory(file.Contents);
        }
        
        gameState->ToneHz = 256;
    }
    
    
    game_controller_input *input0 = &input->Controllers[0];
    if(input0->IsAnalog)
    {
        // analog movement tuning
        gameState->ToneHz = 412 + (int)(240.0f * input0->EndY);
        gameState->XOffset += (int)4.0f * input0->EndX;
    }
    else
    {
        // digital movement tuning
    }
    
    if(input0->Down.EndedDown)
    {
        gameState->YOffset += 1;
    }
    
    GameOutputSound(soundBuffer, gameState->ToneHz);
    GameRender(screenBuffer, gameState->XOffset, gameState->YOffset);
}