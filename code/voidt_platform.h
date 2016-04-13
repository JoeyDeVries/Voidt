/*******************************************************************
** Copyright (C) 2015-2016 {Joey de Vries} {joey.d.vries@gmail.com}
**
** This code is part of Voidt.
** https://github.com/JoeyDeVries/Voidt
**
** Voidt is free software: you can redistribute it and/or modify it
** under the terms of the CC BY-NC 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
*******************************************************************/
#ifndef VOIDT_PLATFORM_H
#define VOIDT_PLATFORM_H




// ----------------------------------------------------------------------------
//      COMPILERS
// ----------------------------------------------------------------------------
#ifndef COMPILER_MSVC
#define COMPILER_MSVC 0
#endif

#ifndef COMPILER_LLVM
#define COMPILER_LLVM 0
#endif

#if !COMPILER_MSVC && !COMPILER_LLVM
    #if _MSC_VER
        #undef COMPILER_MSVC
        #define COMPILER_MSVC 1-4
    #else
        #undef COMPILER_LLVM
        #define COMPILER_LLVM
    #endif
#endif

#if COMPILER_MSVC
    #include <intrin.h>
    // #pragma intrinsic(_BitScanForward)
#endif



// ----------------------------------------------------------------------------
//      STRUCTS: PLATFORM-DEPENDANCY LAYER
// ----------------------------------------------------------------------------
// game threading
struct thread_context
{
    int placeholder;
};

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
    game_button_state MouseButtons[5];
    int32 MouseX, MouseY, MouseZ;

    real32 dtPerFrame;
    
    game_controller_input Controllers[5]; // [0] = keyboard, [1-4] = gamepads
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

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context *thread, void *memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context *thread, char *fileName)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(thread_context *thread, char *fileName, uint32 memorySize, void *memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);
#endif

#define PLATFORM_WRITE_DEBUG_OUTPUT(name) void name(const char *format, ...)
typedef PLATFORM_WRITE_DEBUG_OUTPUT(platform_write_debug_output);

// needs defines from platform function hooks TODO(Joey): clean-up
struct game_memory
{
    bool32 IsInitialized;
    
    int64  PermanentStorageSize;
    void*  PermanentStorage;     // required to be cleared to zero at startup (win32's VirtualAlloc does this by default)
    
    int64 TransientStorageSize;
    void* TransientStorage;
    
    platform_work_queue *QueueHighPriority;
    platform_work_queue *QueueLowPriority;
    
    platform_add_work_entry *PlatformAddEntry;
    platform_complete_all_work *PlatformCompleteAllWork;
    
    debug_platform_free_file_memory  *DEBUGPlatformFreeFileMemory;
    debug_platform_read_entire_file  *DEBUGPlatformReadEntireFile;
    debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;    
    platform_write_debug_output      *PlatformWriteDebugOutput;
};

// NOTE(Joey): encapsulate this without using globals, but still keep it accesible
// for all the game-code (logging tool)
global_variable platform_write_debug_output *GlobalPlatformWriteDebugOutput;

// ----------------------------------------------------------------------------
//      Services that the game provides to the platform layer
// ----------------------------------------------------------------------------
#define GAME_UPDATE_AND_RENDER(name) void name(thread_context* thread, game_memory *memory, game_input *input, game_offscreen_buffer *screenBuffer)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub) { }

#define GAME_GET_SOUND_SAMPLES(name) void name(thread_context *thread, game_memory *memory, game_sound_output_buffer *soundBuffer)
typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples);
GAME_GET_SOUND_SAMPLES(GameGetSoundSamplesStub) { }

#endif