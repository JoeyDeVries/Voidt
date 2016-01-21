#ifndef VOIDT_H
#define VOIDT_H

/*
    Services that the platform layer provides to the game
*/


/*
    Services that the game provides to the platform layer
*/


struct game_offscreen_buffer
{
    void   *Memory;
    uint16  Width;
    uint16  Height;
    uint32  Pitch;
};
// requires four items: timing, input, render buffer, sound buffer
internal void GameUpdateAndRender(game_offscreen_buffer *screenBuffer);


#endif