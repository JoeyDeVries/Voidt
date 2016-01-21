#ifndef VOIDT_H
#define VOIDT_H


// ----------------------------------------------------------------------------
//      PRE-PROCESSOR UTILITY
// ----------------------------------------------------------------------------
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
    bool32 IsAnalog;
    
    real32 StartX;
    real32 StartY;
    
    real32 MinX;
    real32 MinY;
    
    real32 MaxX;
    real32 MaxY;
    
    real32 EndX;
    real32 EndY;
    
    union
    {
        struct
        {
            game_button_state Up;
            game_button_state Down;
            game_button_state Left;
            game_button_state Right;
            game_button_state LeftShoulder;
            game_button_state RightShoulder;
        };
        game_button_state Buttons[6];
    };
};

struct game_input
{
    game_controller_input Controllers[4];
};
    
// ----------------------------------------------------------------------------
//      Services that the platform layer provides to the game
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
//      Services that the game provides to the platform layer
// ----------------------------------------------------------------------------
internal void GameRender(game_offscreen_buffer *screenBuffer, int xOffset, int yOffset);

internal void GameOutputSound(game_sound_output_buffer *soundBuffer, int toneHz);

// requires four items: timing, input, render buffer, sound buffer
internal void GameUpdateAndRender(game_input *input,
                                  game_offscreen_buffer *screenBuffer, 
                                  game_sound_output_buffer *soundBuffer);


#endif