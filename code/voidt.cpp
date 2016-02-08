#include "voidt.h"

#include "voidt_intrinsics.h"


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



internal void CorrectWorldCoord(game_world *world, int32 tileCount, int32 *tileMap, int32 *tile, real32 *tileRel)
{
    int32 tileOverflow = FloorReal32ToInt32(*tileRel / world->TileSideInMeters);
    *tile += tileOverflow;
    *tileRel -= tileOverflow*world->TileSideInMeters;
    
    Assert(*tileRel >= 0);
    Assert(*tileRel <= world->TileSideInMeters);
    
    if(*tile < 0)
    {
        *tile = tileCount + *tile;
        --*tileMap;
    }
    if(*tile >= tileCount)
    {
        *tile = *tile - tileCount;
        ++*tileMap;
    }
}

internal world_position CorrectWorldPosition(game_world *world, world_position pos)
{    
    world_position result = pos;
    
    CorrectWorldCoord(world, world->TileCountX, &result.TileMapX, &result.TileX, &result.TileRelX);
    CorrectWorldCoord(world, world->TileCountY, &result.TileMapY, &result.TileY, &result.TileRelY);
    
    return result;
}

internal bool32 IsWorldPointEmpty(game_world *world, world_position pos)
{
    bool32 empty = false;
        
    tile_map *tileMap = GetTileMap(world, pos.TileMapX, pos.TileMapY);
    empty = IsTileMapPointEmtpy(world, tileMap, pos.TileX, pos.TileY);
       
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
    
    world.TileSideInMeters = 1.4f;
    world.TileSideInPixels = 60;
    world.MetersToPixels = world.TileSideInPixels / world.TileSideInMeters;    
    
    world.LowerLeftX = -world.TileSideInPixels / 2.0f;
    world.LowerLeftY = screenBuffer->Height;
      
    tileMaps[0][0].Tiles = (uint32 *)tiles00;
    tileMaps[1][0].Tiles = (uint32 *)tiles01;
    tileMaps[0][1].Tiles = (uint32 *)tiles10;
    tileMaps[1][1].Tiles = (uint32 *)tiles11; 
    world.TileMaps = (tile_map*)tileMaps;
    
    real32 playerWidth = 0.75f*world.TileSideInMeters;
    real32 playerHeight = (real32)world.TileSideInMeters;
    
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {                  
        gameState->PlayerPos.TileMapX = 0;
        gameState->PlayerPos.TileMapY = 0;        
        gameState->PlayerPos.TileX = 1;
        gameState->PlayerPos.TileY = 3;
        gameState->PlayerPos.TileRelX = 0.5f;
        gameState->PlayerPos.TileRelY = 0.5f;

        memory->IsInitialized = true;
    }
    
    
    tile_map *tileMap = GetTileMap(&world, gameState->PlayerPos.TileMapX, gameState->PlayerPos.TileMapY);
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
                dPlayerY =  1.0f;
            if(controller->MoveDown.EndedDown)
                dPlayerY = -1.0f;
            if(controller->MoveLeft.EndedDown)
                dPlayerX = -1.0f;
            if(controller->MoveRight.EndedDown)
                dPlayerX =  1.0f;
            real32 speed = 2.0f;
            dPlayerX *= speed;
            dPlayerY *= speed;
            
            world_position newPlayerPos = gameState->PlayerPos;
            newPlayerPos.TileRelX += input->dtPerFrame * dPlayerX;
            newPlayerPos.TileRelY += input->dtPerFrame * dPlayerY;
            newPlayerPos = CorrectWorldPosition(&world, newPlayerPos);
            
            world_position playerTop = newPlayerPos;
            playerTop.TileRelY -= 0.5;
            playerTop = CorrectWorldPosition(&world, playerTop);
            
            world_position playerLeft = newPlayerPos;
            playerLeft.TileRelX -= 0.5f*playerWidth;
            playerLeft = CorrectWorldPosition(&world, playerLeft);
            
            world_position playerRight = newPlayerPos;
            playerRight.TileRelX += 0.5f*playerWidth;
            playerRight = CorrectWorldPosition(&world, playerRight);
            
            if(IsWorldPointEmpty(&world, playerTop) &&
               IsWorldPointEmpty(&world, playerLeft) &&
               IsWorldPointEmpty(&world, playerRight))
            {
                gameState->PlayerPos = newPlayerPos;
            }
            
            
           
        // }
    }

    for(int row = 0; row < world.TileCountY; ++row)
    {
        for(int col = 0; col < world.TileCountX; ++col)
        {
            uint32 tileID = GetTileValueUnchecked(&world, tileMap, col, row);
            real32 gray = tileID == 1 ? 1.0f : 0.5f;
         
            
            real32 minX = world.LowerLeftX + ((real32)col * world.TileSideInPixels);
            real32 minY = world.LowerLeftY - ((real32)row * world.TileSideInPixels);
            real32 maxX = minX + world.TileSideInPixels;
            real32 maxY = minY - world.TileSideInPixels;
            DrawRectangle(screenBuffer, minX, maxY, maxX, minY, 0.3f, gray, gray);
            
        }        
    }
    real32 playerLeft = world.LowerLeftX + gameState->PlayerPos.TileX*world.TileSideInPixels + world.MetersToPixels*gameState->PlayerPos.TileRelX  - 0.5f*world.MetersToPixels*playerWidth;
    real32 playerTop = world.LowerLeftY - gameState->PlayerPos.TileY*world.TileSideInPixels - world.MetersToPixels*gameState->PlayerPos.TileRelY - world.MetersToPixels*playerHeight;
    real32 playerR = 1.0f;
    real32 playerG = 0.5f;
    real32 playerB = 0.0f;
   
    
    DrawRectangle(screenBuffer, playerLeft, playerTop, playerLeft + world.MetersToPixels*playerWidth, playerTop + world.MetersToPixels*playerHeight, playerR, playerG, playerB);
  
    GameRender(screenBuffer, gameState);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, 412);
}
