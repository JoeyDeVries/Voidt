#ifndef VOIDT_H
#define VOIDT_H


// ----------------------------------------------------------------------------
//      PRE-PROCESSOR UTILITY
// ----------------------------------------------------------------------------
#if DEBUG
#define Assert(Expression) if(!(Expression)) { *(int *) 0 = 0; }
#else
#define Assert(Expression)
#endif

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

// ----------------------------------------------------------------------------
//      STRUCTS: PLATFORM-INDEPENDENT
// ----------------------------------------------------------------------------
// game sound
struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    int16 *Samples;
};

// screen/render buffer
struct game_offscreen_buffer
{
    void   *Memory;
    uint16  Width;
    uint16  Height;
    uint32  Pitch;
};

// game input state 
struct game_button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
};

struct game_controller_input
{
    bool32 IsConnected;
    bool32 IsAnalog;
    real32 StickAverageX;
    real32 StickAverageY;
    
    union
    {
        struct
        {
            game_button_state MoveUp;
            game_button_state MoveDown;
            game_button_state MoveLeft;
            game_button_state MoveRight;
            
            game_button_state ActionUp;
            game_button_state ActionDown;
            game_button_state ActionLeft;
            game_button_state ActionRight;
            
            game_button_state LeftShoulder;
            game_button_state RightShoulder;
            
            game_button_state Start;
            game_button_state Back;
        };
        game_button_state Buttons[12];
    };
};

struct game_input
{    
    game_controller_input Controllers[5]; // [0] = keyboard, [1-4] = gamepads
};
inline game_controller_input* GetController(game_input *input, uint32 controllerIndex)
{
    Assert(controllerIndex < ArrayCount(input->Controllers));
    game_controller_input *result = &input->Controllers[controllerIndex];
    return result;
}

struct game_memory
{
    bool32 IsInitialized;
    
    int64  PermanentStorageSize;
    void*  PermanentStorage;     // required to be cleared to zero at startup (win32's VirtualAlloc does this by default)
    
    int64 TransientStorageSize;
    void* TransientStorage;
};

struct game_state
{
    int32 ToneHz;
    int32 XOffset;
    int32 YOffset;    
};

    
// ----------------------------------------------------------------------------
//      Services that the platform layer provides to the game
// ----------------------------------------------------------------------------
#if INTERNAL
struct debug_read_file_result
{
    uint32 ContentSize;
    void  *Contents;    
};
internal debug_read_file_result DEBUGPlatformReadEntireFile(char *fileName);
internal void                   DEBUGPlatformFreeFileMemory(void *memory);

internal bool32 DEBUGPlatformWriteEntireFile(char * fileName, uint32 memorySize, void *memory);
#endif


// ----------------------------------------------------------------------------
//      Services that the game provides to the platform layer
// ----------------------------------------------------------------------------
internal void GameRender(game_offscreen_buffer *screenBuffer, int xOffset, int yOffset);

internal void GameOutputSound(game_sound_output_buffer *soundBuffer, int toneHz);

// requires four items: timing, input, render buffer, sound buffer
internal void GameUpdateAndRender(game_memory *memory,
                                  game_input *input,
                                  game_offscreen_buffer *screenBuffer);
                                  
internal void GameGetSoundSamples(game_memory *memory, game_sound_output_buffer *soundBuffer);


#endif