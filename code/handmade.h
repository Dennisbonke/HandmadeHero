#if !defined(HANDMADE_H)

/*
    TODO(Dennis): Services that the platform layer provides to the game.
*/

/*
    NOTE(Dennis): Services that the game provides to the platform layer.
    (this may expand in the future - sound on separate thread, etc.)
*/

/// FOUR THINGS - timing, controller/keyboard input, bitmap buffer to use, sound buffer to use
struct game_offscreen_buffer
{
    /// NOTE(Dennis): Pixels are always 32-bits wide, Memory Order BB GG RR XX
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};
internal void GameUpdateAndRender(game_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset);

#define HANDMADE_H
#endif
