#include "voidt.h"

void GameRender(game_offscreen_buffer *screenBuffer, int xOffset, int yOffset)
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
            uint8 Blue  = (uint8)(X + xOffset);           // B
            uint8 Green = (uint8)(Y + yOffset);           // G
            uint8 Red   = (uint8)(X + yOffset + xOffset); // R
                        
            *Pixel++ = (Red << 16) | (Green << 8) | Blue;
        }
        row += screenBuffer->Pitch;
    }            
}



void GameOutputSound(game_sound_output_buffer *soundBuffer, game_state *gameState, int toneHz)
{
    uint16 toneVolume = 3000;
    // int16 toneHz = 256;
    uint16 wavePeriod = (uint16)(soundBuffer->SamplesPerSecond / toneHz);
    
    int16 *sampleOut = soundBuffer->Samples;
    for(int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
    {
        real32 sineValue = sinf((real32)gameState->tSine);
        int16 sampleValue = (int16)(sineValue * toneVolume);
        *sampleOut++ = sampleValue;
        *sampleOut++ = sampleValue;
        
        gameState->tSine += 2.0f * Pi32 * 1.0f / wavePeriod;
    }        
} 

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize); 
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {        
        char *fileName = __FILE__;
        debug_read_file_result file = memory->DEBUGPlatformReadEntireFile(fileName);
        
        if(file.Contents)
        {
            memory->DEBUGPlatformWriteEntireFile("W:/data/test.out", file.ContentSize, file.Contents);
            memory->DEBUGPlatformFreeFileMemory(file.Contents);
        }
        
        gameState->ToneHz = 412;
        gameState->tSine = 0.0f;
        
        memory->IsInitialized = true;
    }
    
    
    for(int controllerIndex = 0; controllerIndex < ArrayCount(input->Controllers); ++controllerIndex)
    {
        game_controller_input *controller = GetController(input, controllerIndex);
        if(controller->IsAnalog)
        {
            // analog movement tuning
            gameState->ToneHz = 412 + (int)(240.0f * controller->StickAverageY);
            gameState->XOffset += (int)(4.0f * controller->StickAverageX);
        }
        else
        {
            // digital movement tuning
            if(controller->MoveLeft.EndedDown)
            {
                gameState->XOffset -= 1;
            }
            if(controller->MoveRight.EndedDown)
            {
                gameState->XOffset += 1;
            }
        }
        
        if(controller->ActionDown.EndedDown)
        {
            gameState->YOffset += 1;
        }
    }
    
  
    GameRender(screenBuffer, gameState->XOffset, gameState->YOffset);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, gameState->ToneHz);
}
