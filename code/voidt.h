#ifndef VOIDT_H
#define VOIDT_H

#include <stdint.h> // defines precise compiler-independent sizes of primitive types
#include <math.h>

#define internal        static
#define local_persist   static
#define global_variable static

#define Pi32 3.141592665359f

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32   bool32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float  real32;
typedef double real64;


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
    uint32  BytesPerPixel;
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
    game_button_state MouseButtons[5];
    int32 MouseX, MouseY, MouseZ;

    real32 SecondsToAdvanceOverUpdate;
    
    game_controller_input Controllers[5]; // [0] = keyboard, [1-4] = gamepads
};
inline game_controller_input* GetController(game_input *input, uint32 controllerIndex)
{
    Assert(controllerIndex < ArrayCount(input->Controllers));
    game_controller_input *result = &input->Controllers[controllerIndex];
    return result;
}


struct game_state
{
    // int32 ToneHz;
    // int32 XOffset;
    // int32 YOffset;    
    // real32 tSine;
};

struct thread_context
{
    int placeholder;
};


// ----------------------------------------------------------------------------
//      FUNCTION POINTER DEFINITIONS (LIBRARY HOOKS)
// ----------------------------------------------------------------------------

// global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
// #define XInputGetState XInputGetState_


    
// ----------------------------------------------------------------------------
//      Services that the platform layer provides to the game
// ----------------------------------------------------------------------------
#if INTERNAL
struct debug_read_file_result
{
    uint32 ContentSize;
    void  *Contents;    
};

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context *thread, void *memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context *thread, char *fileName)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(thread_context *thread, char *fileName, uint32 memorySize, void *memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);


// debug_read_file_result DEBUGPlatformReadEntireFile(char *fileName);
// void                   DEBUGPlatformFreeFileMemory(void *memory);

// bool32 DEBUGPlatformWriteEntireFile(char * fileName, uint32 memorySize, void *memory);
#endif


// needs defines from platform function hooks TODO(Joey): clean-up
struct game_memory
{
    bool32 IsInitialized;
    
    int64  PermanentStorageSize;
    void*  PermanentStorage;     // required to be cleared to zero at startup (win32's VirtualAlloc does this by default)
    
    int64 TransientStorageSize;
    void* TransientStorage;
    
    debug_platform_free_file_memory  *DEBUGPlatformFreeFileMemory;
    debug_platform_read_entire_file  *DEBUGPlatformReadEntireFile;
    debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
};




// ----------------------------------------------------------------------------
//      Services that the game provides to the platform layer
// ----------------------------------------------------------------------------
void GameRender(game_offscreen_buffer *screenBuffer, int xOffset, int yOffset);

void GameOutputSound(game_sound_output_buffer *soundBuffer, int toneHz);


#define GAME_UPDATE_AND_RENDER(name) void name(thread_context* thread, game_memory *memory, game_input *input, game_offscreen_buffer *screenBuffer)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub) { }

#define GAME_GET_SOUND_SAMPLES(name) void name(thread_context *thread, game_memory *memory, game_sound_output_buffer *soundBuffer)
typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples);
GAME_GET_SOUND_SAMPLES(GameGetSoundSamplesStub) { }


// requires four items: timing, input, render buffer, sound buffer
// void GameUpdateAndRender(game_memory *memory,
                                  // game_input *input,
                                  // game_offscreen_buffer *screenBuffer);
                                  
// void GameGetSoundSamples(game_memory *memory, game_sound_output_buffer *soundBuffer);


#endif