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

inline tile_chunk* GetTileChunk(game_world* world, int32 tileChunkX, int32 tileChunkY)
{
    tile_chunk *tileChunk = 0;
    if(tileChunkX >= 0 && tileChunkX < world->TileChunkCountX && 
       tileChunkY >= 0 && tileChunkY < world->TileChunkCountY)
    {
        tileChunk = &world->TileChunks[tileChunkY*world->TileChunkCountX + tileChunkX];
    }
    return tileChunk;
}

inline uint32 GetTileValueUnchecked(game_world *world, tile_chunk *chunk, uint32 tileX, uint32 tileY)
{
    Assert(chunk);
    Assert(tileX < world->ChunkDim);
    Assert(tileY < world->ChunkDim);
    
    return chunk->Tiles[tileY * world->ChunkDim + tileX];
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


internal void CorrectWorldCoord(game_world *world, uint32 *tile, real32 *tileRel)
{
    int32 tileOverflow = FloorReal32ToInt32(*tileRel / world->TileSideInMeters);
    *tile    += tileOverflow;
    *tileRel -= tileOverflow*world->TileSideInMeters;
    
    Assert(*tileRel >= 0);
    Assert(*tileRel <= world->TileSideInMeters);   
}

internal world_position CorrectWorldPosition(game_world *world, world_position pos)
{    
    world_position result = pos;
    
    CorrectWorldCoord(world, &result.AbsTileX, &result.OffsetX);
    CorrectWorldCoord(world, &result.AbsTileY, &result.OffsetY);
    
    return result;
}

inline tile_chunk_position GetChunkPosition(game_world *world, uint32 absTileX, uint32 absTileY)
{
    tile_chunk_position result;
    
    result.TileChunkX = absTileX >> world->ChunkShift;
    result.TileChunkY = absTileY >> world->ChunkShift;
    result.RelTileX = absTileX & world->ChunkMask;
    result.RelTileY = absTileY & world->ChunkMask;
    
    return result;
}

inline uint32 GetTileValue(game_world *world, tile_chunk *tileChunk, uint32 tileX, uint32 tileY)
{
    uint32 tileValue = 0;
    if(tileChunk)
        tileValue = GetTileValueUnchecked(world, tileChunk, tileX, tileY);
    return tileValue;
}

inline uint32 GetTileValue(game_world *world, uint32 absTileX, uint32 absTileY)
{  
    tile_chunk_position chunkPos = GetChunkPosition(world, absTileX, absTileY);
    tile_chunk *tileChunk = GetTileChunk(world, chunkPos.TileChunkX, chunkPos.TileChunkY);
    uint32 tileChunkValue = GetTileValue(world, tileChunk, chunkPos.RelTileX, chunkPos.RelTileY);
    
    return tileChunkValue;
}


internal bool32 IsWorldPointEmpty(game_world *world, world_position pos)
{
    uint32 tileChunkValue = GetTileValue(world, pos.AbsTileX, pos.AbsTileY);
    bool32 empty = tileChunkValue == 0;
    return empty;
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    // Assert((&input->Controllers[0].Back - &input->Controllers[0].Buttons[0]) == ArrayCount(input->Controllers[0].Buttons) - 1); // check if button array matches union struct members
    Assert(sizeof(game_state) <= memory->PermanentStorageSize); 
    
  
    
        
    uint32 tiles[256][256] = 
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
        {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  1 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0 },  
        {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 0, 0, 0,  1 },  
        {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        {1, 0, 0, 0,  1, 0, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1 },      
    };
    
    
    // uint32 tiles10[9][17] = 
    // {
        // {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1 },  
        // {1, 1, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        // {1, 1, 1, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 0,  1 },  
        // {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  1 },  
        // {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        // {1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        // {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        // {1, 1, 1, 1,  1, 0, 0, 1,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        // {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1 },         
    // };
    
    // uint32 tiles11[9][17] = 
    // {
        // {1, 1, 1, 1,  1, 1, 1, 1,  0, 1, 1, 1,  1, 1, 1, 1,  1 },  
        // {1, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  0, 1, 0, 0,  1 },  
        // {1, 1, 0, 0,  0, 0, 0, 0,  0, 1, 0, 0,  0, 1, 1, 0,  1 },  
        // {1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,  0, 0, 1, 1,  1 },  
        // {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        // {1, 1, 0, 0,  0, 0, 0, 0,  1, 0, 0, 0,  0, 1, 1, 0,  1 },  
        // {1, 1, 0, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1 },  
        // {1, 1, 1, 1,  1, 0, 0, 1,  0, 0, 0, 0,  0, 1, 0, 0,  1 },  
        // {1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1 },         
    // };
          
    game_world world = {};
    world.ChunkShift = 8;
    world.ChunkMask = 0xFF;
    world.ChunkDim = 256;
    
    world.TileChunkCountX = 1;
    world.TileChunkCountY = 1;

    tile_chunk tileChunk;
    tileChunk.Tiles = (uint32 *)tiles;
    world.TileChunks = &tileChunk;
    
    world.TileSideInMeters = 1.4f;
    world.TileSideInPixels = 60;
    world.MetersToPixels = world.TileSideInPixels / world.TileSideInMeters;    


    real32 playerWidth = 0.75f*world.TileSideInMeters;
    real32 playerHeight = (real32)world.TileSideInMeters;
    
    
    game_state *gameState = (game_state*)memory->PermanentStorage;    
    if(!memory->IsInitialized)
    {                  
        gameState->PlayerPos.AbsTileX = 3;
        gameState->PlayerPos.AbsTileY = 3;        
        gameState->PlayerPos.OffsetX = 0.5f;
        gameState->PlayerPos.OffsetY = 0.5f;

        memory->IsInitialized = true;
    }
    
  
    
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
            newPlayerPos.OffsetX += input->dtPerFrame * dPlayerX;
            newPlayerPos.OffsetY += input->dtPerFrame * dPlayerY;
            newPlayerPos = CorrectWorldPosition(&world, newPlayerPos);
            
            world_position playerTop = newPlayerPos;
            playerTop.OffsetY += 0.25;
            playerTop = CorrectWorldPosition(&world, playerTop);
            
            world_position playerLeft = newPlayerPos;
            playerLeft.OffsetX -= 0.5f*playerWidth;
            playerLeft = CorrectWorldPosition(&world, playerLeft);
            
            world_position playerRight = newPlayerPos;
            playerRight.OffsetX += 0.5f*playerWidth;
            playerRight = CorrectWorldPosition(&world, playerRight);
            
            if(IsWorldPointEmpty(&world, playerTop) &&
               IsWorldPointEmpty(&world, playerLeft) &&
               IsWorldPointEmpty(&world, playerRight))
            {
                gameState->PlayerPos = newPlayerPos;
            }
            
            
           
        // }
    }    
        
    real32 LowerLeftX = -world.TileSideInPixels / 2.0f;
    real32 LowerLeftY = screenBuffer->Height;
    
    real32 centerX = 0.5f*(real32)screenBuffer->Width;
    real32 centerY = 0.5f*(real32)screenBuffer->Height;

    for(int32 relRow = -10; relRow < 10; ++relRow)
    {
        for(int32 relCol = -20; relCol < 20; ++relCol)
        {
            uint32 col = relCol + gameState->PlayerPos.AbsTileX;
            uint32 row = relRow + gameState->PlayerPos.AbsTileY;
            uint32 tileID = GetTileValue(&world, col, row);
            real32 gray = tileID == 1 ? 1.0f : 0.5f;
            
            if(col == gameState->PlayerPos.AbsTileX && row == gameState->PlayerPos.AbsTileY)
                gray = 0.25f;
         
            
            real32 minX = centerX + ((real32)relCol * world.TileSideInPixels);
            real32 minY = centerY - ((real32)relRow * world.TileSideInPixels);
            real32 maxX = minX + world.TileSideInPixels;
            real32 maxY = minY - world.TileSideInPixels;
            DrawRectangle(screenBuffer, minX, maxY, maxX, minY, 0.3f, gray, gray);
            
        }        
    }
    real32 playerLeft = centerX + world.MetersToPixels*gameState->PlayerPos.OffsetX  - 0.5f*world.MetersToPixels*playerWidth;
    real32 playerTop = centerY - world.MetersToPixels*gameState->PlayerPos.OffsetY - world.MetersToPixels*playerHeight;
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
