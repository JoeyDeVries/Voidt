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
    // NOTE(Joey): samples must be padded to be a multiple of 4
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

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(void *memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(char *fileName)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(char *fileName, uint32 memorySize, void *memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);
#endif

#define PLATFORM_WRITE_DEBUG_OUTPUT(name) void name(const char *format, ...)
typedef PLATFORM_WRITE_DEBUG_OUTPUT(platform_write_debug_output);

/* NOTE(Joey): 
  
  Won't contain file handle as this is OS specific (cast to OS specific
  file_handle when using this struct that does contain the file handle. As long as 
  OS file handle struct contains the platform_file_handle as the first element the
  subsequent memory of the platform_file_handle will always point to OS handle that
  OS file function can use. This is C-alternative to polymorphism.
  
*/
struct platform_file_handle 
{
    b32 HasErrors;
};

typedef platform_file_handle* platform_open_file(char *fileName);
typedef void                  platform_read_file(platform_file_handle *file, u64 offset, u64 size, void* dest);
typedef void                  platform_close_file(platform_file_handle *file);

struct platform_api
{
    // threading
    platform_work_queue *WorkQueueHighPriority;
    platform_work_queue *WorkQueueLowPriority;
    
    platform_add_work_entry    *AddWorkEntry;
    platform_complete_all_work *CompleteAllWork;
    
    // file management
    debug_platform_free_file_memory  *DEBUGFreeFileMemory;
    debug_platform_read_entire_file  *DEBUGReadEntireFile;
    debug_platform_write_entire_file *DEBUGWriteEntireFile;    
    
    platform_open_file  *OpenFile;
    platform_read_file  *ReadFile;
    platform_close_file *CloseFile;
    
    // output
    platform_write_debug_output *WriteDebugOutput;    
};

// needs defines from platform function hooks TODO(Joey): clean-up
struct game_memory
{
    bool32 IsInitialized;
    
    int64  PermanentStorageSize;
    void*  PermanentStorage;     // required to be cleared to zero at startup (win32's VirtualAlloc does this by default)
    
    int64 TransientStorageSize;
    void* TransientStorage;
    
    platform_api PlatformAPI;
    
    // platform_work_queue *WorkQueueHighPriority;
    // platform_work_queue *WorkQueueLowPriority;
    
    // platform_add_work_entry *PlatformAddWorkEntry;
    // platform_complete_all_work *PlatformCompleteAllWork;
    
    // debug_platform_free_file_memory  *DEBUGPlatformFreeFileMemory;
    // debug_platform_read_entire_file  *DEBUGPlatformReadEntireFile;
    // debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;    
    // platform_write_debug_output      *PlatformWriteDebugOutput;
};

// NOTE(Joey): encapsulate this without using globals, but still keep it accesible
// for all the game-code (logging tool)
global_variable platform_api PlatformAPI;

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