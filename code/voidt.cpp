#include "voidt.h"

#include "intrinsics.h"

#include "map.cpp"

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

// internal bool32 IsTileEmtpy(game_world *world, tile_map *tileMap, int32 testTileX, int32 testTileY)
// {
    // bool32 empty = false;
    
    // if(tileMap)
    // {
        // if(testTileX >= 0 && testTileX < world->TileCountX && testTileY >=0 && testTileY < world->TileCountY)
        // {
            // empty = GetTileValueUnchecked(world, tileMap, testTileX, testTileY) == 0;       
        // }
    // }
    // return empty;    
    
    
// }



extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize); 
    
  
    
        
    // uint32 tiles[256][256] = 
    // {
        // {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1},  
        // {1, 1, 0, 0,  0, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1,  1, 1, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1},  
        // {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 1, 0,  1,  1, 1, 1, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 0,  1},  
        // {1, 0, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 1,  1,  1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  1},  
        // {1, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1},  
        // {1, 1, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  0, 1, 0, 0,  1,  1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1},  
        // {1, 0, 0, 0,  0, 1, 0, 0,  1, 0, 0, 0,  1, 0, 0, 0,  1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1},  
        // {1, 1, 1, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1,  1, 1, 1, 1,  1, 0, 0, 1,  0, 0, 0, 0,  0, 1, 0, 0,  1},  
        // {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1,  1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1},         
        // {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1,  1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1},  
        // {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1,  1, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  0, 1, 0, 0,  1},  
        // {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1,  1, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  0, 1, 1, 0,  1},  
        // {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 1, 1,  1},  
        // {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1},  
        // {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0,  1,  1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 1, 1, 0,  1},  
        // {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1,  1, 1, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1},  
        // {1, 0, 0, 0,  1, 0, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,  1,  1, 1, 1, 1,  1, 0, 0, 1,  0, 0, 0, 0,  0, 1, 0, 0,  1},  
        // {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1},      
    // };




    real32 playerWidth = 0.75f*1.4f;
    real32 playerHeight = 1.4f;
    
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {                  
        gameState->PlayerPos.AbsTileX = 1;
        gameState->PlayerPos.AbsTileY = 3;       
        gameState->PlayerPos.AbsTileZ = 0;          
        gameState->PlayerPos.OffsetX = 5.0f;
        gameState->PlayerPos.OffsetY = 5.0f;
        
        // generate world tile map          
        InitializeArena(&gameState->WorldArena, memory->PermanentStorageSize - sizeof(game_state), (uint8*)memory->PermanentStorage + sizeof(game_state));
        
        gameState->World = PushStruct(&gameState->WorldArena, game_world);
        game_world *world = gameState->World;
        world->TileMap = PushStruct(&gameState->WorldArena, tile_map);
                
        tile_map *tileMap = world->TileMap;
        
        tileMap->ChunkShift = 4;
        tileMap->ChunkMask = (1 << tileMap->ChunkShift) - 1;
        tileMap->ChunkDim = (1 << tileMap->ChunkShift);
        
        tileMap->TileChunkCountX = 128;
        tileMap->TileChunkCountY = 128;
        tileMap->TileChunkCountZ = 2;
        tileMap->TileChunks = PushArray(&gameState->WorldArena, tileMap->TileChunkCountX*tileMap->TileChunkCountY*tileMap->TileChunkCountZ, tile_chunk);
        // for(uint32 Y = 0; Y < tileMap->TileChunkCountY; ++Y)
        // {
            // for(uint32 X = 0; X < tileMap->TileChunkCountX; ++X)
            // {
                // tileMap->TileChunks[Y*tileMap->TileChunkCountX + X].Tiles = PushArray(&gameState->WorldArena, tileMap->ChunkDim*tileMap->ChunkDim, uint32);
            // }
        // }
        
        
        tileMap->TileSideInMeters = 1.4f;

        uint32 tilesPerWidth = 17;
        uint32 tilesPerHeight = 9;
        
        uint32 screenX = 0;
        uint32 screenY = 0;
        uint32 absTileZ = 0;
        bool doorTop = false;
        bool doorBottom = false;
        bool doorLeft = false;
        bool doorRight = false;
        bool doorUp  = false;
        bool doorDown = false;
        for(uint32 screenIndex = 0; screenIndex < 100; ++screenIndex)
        {
            uint32 randomChoice = 0;

            if(doorUp || doorDown)
            {
                randomChoice = (885443 * screenIndex + (screenIndex + 7) * 7634 / 987) % 2;
            }
            else
            {
                randomChoice = (885443 * screenIndex + (screenIndex + 7) * 7634 / 987) % 3;
            }
            
            if(randomChoice == 2)
            {
                if (absTileZ == 0)
                {
                    doorUp = true;
                }
                else
                {
                    doorDown = true;
                }
            }                
            else if (randomChoice == 1)
                doorRight = true;
            else
                doorTop = true;
            
            
            for(uint32 tileY = 0; tileY < tilesPerHeight; ++tileY)
            {
                for(uint32 tileX = 0; tileX< tilesPerWidth; ++tileX)
                {
                    uint32 absTileX = screenX*tilesPerWidth + tileX;
                    uint32 absTileY = screenY*tilesPerHeight + tileY;
                    
                    uint32 tileValue = 1;
                    if(tileX == 0 && (!doorLeft || tileY != (tilesPerHeight / 2)))
                        tileValue = 2;
                    if(tileX == (tilesPerWidth - 1) && (!doorRight || tileY != (tilesPerHeight / 2)))
                        tileValue = 2;
                    if(tileY == 0 && (!doorBottom || tileX != (tilesPerWidth / 2)))
                        tileValue = 2;
                    if(tileY == (tilesPerHeight -1) && (!doorTop || tileX != (tilesPerWidth / 2)))
                        tileValue = 2;
                    
                    if(tileX == 10 && tileY == 6)
                    {
                        if(doorUp) 
                        {
                            tileValue = 3;
                        }
                        if(doorDown)
                        {
                            tileValue = 4;
                        }
                    }                      
                    
                    SetTileValue(&gameState->WorldArena, tileMap, absTileX, absTileY, absTileZ, tileValue);
                }
            }
            
            if(doorUp)
            {
                doorUp = false;
                doorDown = true;
            }
            else if(doorDown)
            {
                doorUp = true;
                doorDown = false;
            }
            else
            {
                doorUp = false;
                doorDown = false;
            }
            
            doorLeft = doorRight;
            doorBottom = doorTop;
            
            doorRight = false;
            doorTop = false;
            
            if(randomChoice == 2)
            {
                if(absTileZ == 0) 
                {
                    absTileZ = 1;
                }
                else 
                {
                    absTileZ = 0;
                }
            }
            else if(randomChoice == 1)
                screenX++;
            else
                screenY++;
        }

        memory->IsInitialized = true;
    }
    
  
    
    DrawRectangle(screenBuffer, 0.0f, 0.0f, (real32)screenBuffer->Width, (real32)screenBuffer->Height, 1.0f, 0.5f, 0.0f);
    
    tile_map *tileMap = gameState->World->TileMap;
    
    
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
            if(input->Controllers[1].ActionDown.EndedDown)
                speed = 10.0f;
            dPlayerX *= speed;
            dPlayerY *= speed;
            
            tile_map_position newPlayerPos = gameState->PlayerPos;
            newPlayerPos.OffsetX += input->dtPerFrame * dPlayerX;
            newPlayerPos.OffsetY += input->dtPerFrame * dPlayerY;
            newPlayerPos = CorrectTileMapPosition(tileMap, newPlayerPos);
            
            tile_map_position playerTop = newPlayerPos;
            playerTop.OffsetY += 0.25;
            playerTop = CorrectTileMapPosition(tileMap, playerTop);
            
            tile_map_position playerLeft = newPlayerPos;
            playerLeft.OffsetX -= 0.5f*playerWidth;
            playerLeft = CorrectTileMapPosition(tileMap, playerLeft);
            
            tile_map_position playerRight = newPlayerPos;
            playerRight.OffsetX += 0.5f*playerWidth;
            playerRight = CorrectTileMapPosition(tileMap, playerRight);
            
            if(IsTileMapPointEmpty(tileMap, playerTop) &&
               IsTileMapPointEmpty(tileMap, playerLeft) &&
               IsTileMapPointEmpty(tileMap, playerRight))
            {
                gameState->PlayerPos = newPlayerPos;
            }
            
            
           
        // }
    }    
        
    int32  TileSideInPixels= 60;
    real32 MetersToPixels = TileSideInPixels / tileMap->TileSideInMeters;       
        
    real32 LowerLeftX = -TileSideInPixels / 2.0f;
    real32 LowerLeftY = screenBuffer->Height;
    
    real32 screenCenterX = 0.5f*(real32)screenBuffer->Width;
    real32 screenCenterY = 0.5f*(real32)screenBuffer->Height;

    for(int32 relRow = -10; relRow < 10; ++relRow)
    {
        for(int32 relCol = -20; relCol < 20; ++relCol)
        {
            uint32 col = relCol + gameState->PlayerPos.AbsTileX;
            uint32 row = relRow + gameState->PlayerPos.AbsTileY;
            uint32 tileID = GetTileValue(tileMap, col, row, gameState->PlayerPos.AbsTileZ);
            
            if(tileID > 0)
            {
                real32 gray = tileID == 2 ? 1.0f : 0.5f;
                
                if(tileID > 2)
                    gray = 0.1f;
                
                if(col == gameState->PlayerPos.AbsTileX && row == gameState->PlayerPos.AbsTileY)
                    gray = 0.25f;


             
                
                real32 cenX = screenCenterX - MetersToPixels*gameState->PlayerPos.OffsetX + ((real32)relCol * TileSideInPixels);
                real32 cenY = screenCenterY + MetersToPixels*gameState->PlayerPos.OffsetY - ((real32)relRow * TileSideInPixels);
                real32 minX = cenX - 0.5f*TileSideInPixels; 
                real32 minY = cenY - 0.5f*TileSideInPixels;
                real32 maxX = cenX + 0.5f*TileSideInPixels;
                real32 maxY = cenY + 0.5f*TileSideInPixels;
                DrawRectangle(screenBuffer, minX, minY, maxX, maxY, 0.3f, gray, gray);
            }
            
        }        
    }
    real32 playerLeft = screenCenterX - 0.5f*MetersToPixels*playerWidth;
    real32 playerTop = screenCenterY - MetersToPixels*playerHeight;
    real32 playerR = 1.0f;
    real32 playerG = 0.5f;
    real32 playerB = 0.0f;       
    DrawRectangle(screenBuffer, playerLeft, playerTop, playerLeft + MetersToPixels*playerWidth, playerTop + MetersToPixels*playerHeight, playerR, playerG, playerB);
  
    GameRender(screenBuffer, gameState);
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    game_state *gameState = (game_state*)memory->PermanentStorage;      
    GameOutputSound(soundBuffer, gameState, 412);
}
