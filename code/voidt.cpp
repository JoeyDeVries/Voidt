#include "voidt.h"


inline int32 RoundReal32ToInt32(real32 value)
{
    return (int32)(value + 0.5f);
}

inline uint32 RoundReal32ToUint32(real32 value)
{
    return (uint32)(value + 0.5f);
}

#include "math.h"
inline int32 FloorReal32ToInt32(real32 value)
{
    return (uint32)floorf(value);
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

inline tile_map* GetTileMap(game_world* world, int32 tileMapX, int32 tileMapY)
{
    tile_map *tileMap = 0;
    if(tileMapX >= 0 && tileMapX < world->CountX && 
       tileMapY >= 0 && tileMapY < world->CountY)
    {
        tileMap = &world->TileMaps[tileMapY*world->CountX + tileMapX];
    }
    return tileMap;
}

inline uint32 GetTileValueUnchecked(game_world *world, tile_map *tileMap, int32 tileX, int32 tileY)
{
    Assert(tileMap);
    return tileMap->Tiles[tileY * world->TileCountX + tileX];
}

internal bool32 IsTileMapPointEmtpy(game_world *world, tile_map *tileMap, int32 testTileX, int32 testTileY)
{
    bool32 empty = false;
    
    if(tileMap)
    {
        if(testTileX >= 0 && testTileX < world->TileCountX && testTileY >=0 && testTileY < world->TileCountY)
        {
            empty = GetTileValueUnchecked(world, tileMap, testTileX, testTileY) == 0;       
        }
    }
    return empty;    
}



internal canonical_position GetCanonicalPosition(game_world *world, raw_position pos)
{    
    canonical_position result;
    
    result.TileMapX = pos.TileMapX;
    result.TileMapY = pos.TileMapY;
    
    real32 X = pos.X - world->UpperLeftX;
    real32 Y = pos.Y - world->UpperLeftY;
    result.TileX = FloorReal32ToInt32(X / world->TileWidth);
    result.TileY = FloorReal32ToInt32(Y / world->TileHeight);
    
    result.X = X - result.TileX*world->TileWidth;
    result.Y = Y - result.TileY*world->TileHeight;
    
    Assert(result.X >= 0);
    Assert(result.Y >= 0);
    Assert(result.X < world->TileWidth);
    Assert(result.Y < world->TileHeight);
    
    if(result.TileX < 0)
    {
        result.TileX = world->TileCountX + result.TileX;
        --result.TileMapX;
    }
    if(result.TileY < 0)
    {
        result.TileY = world->TileCountY + result.TileY;
        --result.TileMapY;
    }
    if(result.TileX >= world->TileCountX)
    {
        result.TileX = result.TileX - world->TileCountX;
        ++result.TileMapX;
    }
    if(result.TileY >= world->TileCountY)
    {
        result.TileY = result.TileY - world->TileCountY;
        ++result.TileMapY;
    }
    
    return result;
}

internal bool32 IsWorldPointEmpty(game_world *world, raw_position pos)
{
    bool32 empty = false;
        
    canonical_position canPos = GetCanonicalPosition(world, pos);    
    tile_map *tileMap = GetTileMap(world, canPos.TileMapX, canPos.TileMapY);
    empty = IsTileMapPointEmtpy(world, tileMap, canPos.TileX, canPos.TileY);
       
    return empty;
    
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize); 
    
  
    
        
    uint32 tiles00[9][17] = 
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1 },  
        {1, 1, 0, 0,  0, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0,  1 },  
        {1, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 1,  1 },  
        {1, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0,  0 },  
        {1, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 0, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0,  1 },  
        {1, 1, 1, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1 },         
    };
    
    uint32 tiles01[9][17] = 
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 0,  1 },  
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        {1, 1, 1, 1,  1, 0, 0, 1,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1 },         
    };
    
    uint32 tiles10[9][17] = 
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1 },  
        {1, 1, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 1, 1, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 0,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  1 },  
        {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        {1, 1, 1, 1,  1, 0, 0, 1,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1 },         
    };
    
    uint32 tiles11[9][17] = 
    {
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  0, 1, 1, 0,  1 },  
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 1, 1,  1 },  
        {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 1, 1, 0,  1 },  
        {1, 1, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        {1, 1, 1, 1,  1, 0, 0, 1,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1 },         
    };
    
    tile_map tileMaps[2][2];
      
    game_world world = {};
    world.CountX = 2;
    world.CountY = 2;
        
    world.TileCountX = 17;
    world.TileCountY = 9;
    
    world.UpperLeftX = -30;
    world.UpperLeftY = 0;
    world.TileWidth = 60;
    world.TileHeight = 60;
      
    tileMaps[0][0].Tiles = (uint32 *)tiles00;
    tileMaps[1][0].Tiles = (uint32 *)tiles01;
    tileMaps[0][1].Tiles = (uint32 *)tiles10;
    tileMaps[1][1].Tiles = (uint32 *)tiles11; 
    world.TileMaps = (tile_map*)tileMaps;
    
    real32 playerWidth = 0.75f*world.TileWidth;
    real32 playerHeight = world.TileHeight;
    
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {                       
        gameState->PlayerX = 200.0f;
        gameState->PlayerY = 200.0f;
        
        gameState->PlayerTileMapX = 0;
        gameState->PlayerTileMapY = 0;
        memory->IsInitialized = true;
    }
    
    
    tile_map *tileMap = GetTileMap(&world, gameState->PlayerTileMapX, gameState->PlayerTileMapY);
    Assert(tileMap);
    
    
  
    
    DrawRectangle(screenBuffer, 0.0f, 0.0f, (real32)screenBuffer->Width, (real32)screenBuffer->Height, 1.0f, 0.5f, 0.0f);
    
    
    
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
                dPlayerY = -1.0f;
            if(controller->MoveDown.EndedDown)
                dPlayerY =  1.0f;
            if(controller->MoveLeft.EndedDown)
                dPlayerX = -1.0f;
            if(controller->MoveRight.EndedDown)
                dPlayerX =  1.0f;
            real32 speed = 128.0f;
            dPlayerX *= speed;
            dPlayerY *= speed;
            
            real32 newPlayerX = gameState->PlayerX + input->dtPerFrame * dPlayerX;
            real32 newPlayerY = gameState->PlayerY + input->dtPerFrame * dPlayerY;
            
            raw_position playerPos   = { gameState->PlayerTileMapX, gameState->PlayerTileMapY, newPlayerX, newPlayerY };
            raw_position playerTop   = { gameState->PlayerTileMapX, gameState->PlayerTileMapY, newPlayerX, newPlayerY - 17 };
            raw_position playerLeft  = { gameState->PlayerTileMapX, gameState->PlayerTileMapY, newPlayerX - 0.5f*playerWidth, newPlayerY};
            raw_position playerRight = { gameState->PlayerTileMapX, gameState->PlayerTileMapY, newPlayerX + 0.5f*playerWidth, newPlayerY };
            
            if(IsWorldPointEmpty(&world, playerTop) &&
               IsWorldPointEmpty(&world, playerLeft) &&
               IsWorldPointEmpty(&world, playerRight))
            {
                canonical_position canPos = GetCanonicalPosition(&world, playerPos);
                
                gameState->PlayerTileMapX = canPos.TileMapX;
                gameState->PlayerTileMapY = canPos.TileMapY;
                gameState->PlayerX = world.UpperLeftX + world.TileWidth*canPos.TileX + canPos.X;
                gameState->PlayerY = world.UpperLeftY + world.TileHeight*canPos.TileY + canPos.Y;
            }
            
            
           
        // }
    }

    for(int row = 0; row < world.TileCountY; ++row)
    {
        for(int col = 0; col < world.TileCountX; ++col)
        {
            uint32 tileID = GetTileValueUnchecked(&world, tileMap, col, row);
            real32 gray = tileID == 1 ? 1.0f : 0.5f;
         
            
            real32 minX = world.UpperLeftX + ((real32)col * world.TileWidth);
            real32 minY = world.UpperLeftY + ((real32)row * world.TileHeight);
            real32 maxX = minX + world.TileWidth;
            real32 maxY = minY + world.TileHeight;
            DrawRectangle(screenBuffer, minX, minY, maxX, maxY, 0.3f, gray, gray);
            
        }        
    }
    real32 playerLeft = gameState->PlayerX - 0.5f*playerWidth;
    real32 playerTop = gameState->PlayerY - playerHeight;    
    real32 playerR = 1.0f;
    real32 playerG = 0.5f;
    real32 playerB = 0.0f;
   
    
    DrawRectangle(screenBuffer, playerLeft, playerTop, playerLeft + playerWidth, playerTop + playerHeight, playerR, playerG, playerB);
  
    GameRender(screenBuffer, gameState);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, 412);
}
