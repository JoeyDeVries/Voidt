#include "voidt.h"


internal int32 RoundReal32ToInt32(real32 value)
{
    return (int32)(value + 0.5f);
}

internal uint32 RoundReal32ToUint32(real32 value)
{
    return (uint32)(value + 0.5f);
}

internal void DrawRectangle(game_offscreen_buffer *buffer, real32 realMinX, real32 realMinY, real32 realMaxX, real32 realMaxY, real32 r, real32 g, real32 b)
{
    int32 minX = RoundReal32ToInt32(realMinX);
    int32 minY = RoundReal32ToInt32(realMinY);
    int32 maxX = RoundReal32ToInt32(realMaxX);
    int32 maxY = RoundReal32ToInt32(realMaxY);
    
    if(minX < 0) minX = 0;
    if(minY < 0) minY = 0;
    if(maxX > buffer->Width) maxX = buffer->Width;
    if(maxY > buffer->Height) maxY = buffer->Height;
    
    // BIT PATTERN: 0x AA RR GG BB
    uint32 color = (RoundReal32ToUint32(r * 255.0f) << 16) |
                   (RoundReal32ToUint32(g * 255.0f) << 8) |
                   (RoundReal32ToUint32(b * 255.0f) << 0);
        
    uint8* row = (uint8*)buffer->Memory + minX * buffer->BytesPerPixel + minY * buffer->Pitch;
    
    for(int y = minY; y < maxY; ++y)
    {
        uint32* pixel = (uint32*)row;
        for(int x = minX; x < maxX; ++x)
        {            
            *pixel++ = color;        
        }          
        row += buffer->Pitch;
    }
    
}

void GameRender(game_offscreen_buffer *buffer, game_state *state)
{
    // int XOffset = 0;
    // int YOffset = 0;
    // render
    // uint8 *row = (uint8 *)screenBuffer->Memory; 
    // for(int Y = 0; Y < screenBuffer->Height; ++Y)
    // {
        // uint32 *Pixel = (uint32 *)row;
        // for(int X = 0; X < screenBuffer->Width; ++X)
        // {    
            // uint8 Blue  = (uint8)(X + xOffset);           // B
            // uint8 Green = (uint8)(Y + yOffset);           // G
            // uint8 Red   = (uint8)(X + yOffset + xOffset); // R
                        
            // *Pixel++ = (Red << 16) | (Green << 8) | Blue;
        // }
        // row += screenBuffer->Pitch;
    // }           

    uint32 tileMap[9][17] = 
    {
        {0, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0,  1 },  
        {0, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {0, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0,  1 },  
        {0, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 1,  1 },  
        {0, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0,  1 },  
        {0, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {0, 0, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0,  1 },  
        {0, 1, 1, 1,  1, 0, 0, 1,  1, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {0, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0,  1 },         
    };
    
    DrawRectangle(buffer, 0.0f, 0.0f, (real32)buffer->Width, (real32)buffer->Height, 1.0f, 0.5f, 0.0f);
    
    real32 upperLeftX = -30;
    real32 upperLeftY = 0;
    real32 tileWidth = 60;
    real32 tileHeight = 60;
    
    for(int row = 0; row < 9; ++row)
    {
        for(int col = 0; col < 17; ++col)
        {
            uint32 tileID = tileMap[row][col];
            real32 gray = tileID == 1 ? 1.0f : 0.5f;
         
            
            real32 minX = upperLeftX + ((real32)col * tileWidth);
            real32 minY = upperLeftY + ((real32)row * tileHeight);
            real32 maxX = minX + tileWidth;
            real32 maxY = minY + tileHeight;
            DrawRectangle(buffer, minX, minY, maxX, maxY, 0.3f, gray, gray);
            
        }        
    }
    
    real32 playerR = 1.0f;
    real32 playerG = 0.5f;
    real32 playerB = 0.0f;
    real32 playerWidth = 0.75f*tileWidth;
    real32 playerHeight = tileHeight;
    real32 playerLeft = state->PlayerX - 0.5f*playerWidth;
    real32 playerTop = state->PlayerY - playerHeight;
    
    DrawRectangle(buffer, playerLeft, playerTop, playerLeft + playerWidth, playerTop + playerHeight, playerR, playerG, playerB);
}



void GameOutputSound(game_sound_output_buffer *soundBuffer, game_state *gameState, int toneHz)
{
    // uint16 toneVolume = 3000;
    // uint16 wavePeriod = (uint16)(soundBuffer->SamplesPerSecond / toneHz);
    
    // int16 *sampleOut = soundBuffer->Samples;
    // for(int sampleIndex = 0; sampleIndex < soundBuffer->SampleCount; ++sampleIndex)
    // {
        // real32 sineValue = sinf((real32)gameState->tSine);
        // int16 sampleValue = (int16)(sineValue * toneVolume);
        // sampleValue = 0; // disable
        // *sampleOut++ = sampleValue;
        // *sampleOut++ = sampleValue;
        
        // gameState->tSine += 2.0f * Pi32 * 1.0f / wavePeriod;
    // }        
} 


extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize); 
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {                       
        gameState->PlayerX = 200.0f;
        gameState->PlayerY = 200.0f;
        memory->IsInitialized = true;
    }
    
    
    for(int controllerIndex = 0; controllerIndex < ArrayCount(input->Controllers); ++controllerIndex)
    {
        game_controller_input *controller = GetController(input, controllerIndex);
        if(controller->IsAnalog)
        {
            // analog movement tuning
        }
        // else
        // {
            // digital movement tuning
            real32 dPlayerX = 0.0f;
            real32 dPlayerY = 0.0f;
            
            if(controller->MoveUp.EndedDown)
                dPlayerY = -50.0f;
            if(controller->MoveDown.EndedDown)
                dPlayerY =  50.0f;
            if(controller->MoveLeft.EndedDown)
                dPlayerX = -50.0f;
            if(controller->MoveRight.EndedDown)
                dPlayerX =  50.0f;
            
            gameState->PlayerX += input->dtPerFrame * dPlayerX;
            gameState->PlayerY += input->dtPerFrame * dPlayerY;
        // }
    }
    
  
    GameRender(screenBuffer, gameState);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, 412);
}
