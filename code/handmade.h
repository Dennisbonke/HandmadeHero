#if !defined(HANDMADE_H)

/*
   NOTE(Dennis):
HANDMADE_INTERNAL:
  0 - Build for public release
  1 - Build for developer only

HANDMADE_SLOW:
  0 - Not slow code allowed!
  1 - Slow code welcome!
*/

#if HANDMADE_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif // HANDMADE_SLOW

#define Kilobytes(Value) ((Value)*1024)
#define Megabytes(Value) (Kilobytes(Value)*1024)
#define Gigabytes(Value) (Megabytes(Value)*1024)
#define Terabytes(Value) (Gigabytes(Value)*1024)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
/// TODO(Dennis): swap, min, max ... macros???

/*
    TODO(Dennis): Services that the platform layer provides to the game.
*/

/*
    NOTE(Dennis): Services that the game provides to the platform layer.
    (this may expand in the future - sound on separate thread, etc.)
*/

/// FOUR THINGS - timing, controller/keyboard input, bitmap buffer to use, sound buffer to use

/// TODO(Dennis): In the future, rendering _specifically_ will become a three-tiered abstraction!!!
struct game_offscreen_buffer
{
    /// NOTE(Dennis): Pixels are always 32-bits wide, Memory Order BB GG RR XX
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    int16 *Samples;

};

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
        game_button_state Buttons[6];
        struct
        {
            game_button_state Up;
            game_button_state Down;
            game_button_state Left;
            game_button_state Right;
            game_button_state LeftShoulder;
            game_button_state RightShoulder;
        };
    };
};

struct game_input
{
    /// TODO(Dennis): Insert clock values here.
    game_controller_input Controllers[4];
};

struct game_memory
{
    bool32 IsInitialized;

    uint64 PermanentStorageSize;
    void *PermanentStorage; /// NOTE(Dennis): REQUIRED to be cleared to zero at startup

    uint64 TransientStorageSize;
    void *TransientStorage; /// NOTE(Dennis): REQUIRED to be cleared to zero at startup
};

internal void GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer,
                                  game_sound_output_buffer *SoundBuffer);

///
///
///

struct game_state
{
    int ToneHz;
    int GreenOffset;
    int BlueOffset;
};

#define HANDMADE_H
#endif
