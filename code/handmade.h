#if !defined(HANDMADE_H)

/**
   NOTE(Dennis):
HANDMADE_INTERNAL:
  0 - Build for public release
  1 - Build for developer only

HANDMADE_SLOW:
  0 - No slow code allowed!
  1 - Slow code welcome!
*/

#if HANDMADE_SLOW
/// TODO(Dennis): Complete assertion macro - don't worry everyone!
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif // HANDMADE_SLOW

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
/// TODO(Dennis): swap, min, max ... macros???

inline uint32
SafeTruncateUInt64(uint64 Value)
{
    /// TODO(Dennis): Defines for maximum values
    Assert(Value <= 0xFFFFFFFF);
    uint32 Result = (uint32)Value;
    return(Result);
}

/**
    NOTE(Dennis): Services that the platform layer provides to the game.
*/

#if HANDMADE_INTERNAL
/// NOTE(Dennis): These are NOT for doing anything in the shipping game - they are blocking and the write does not protect against lost data!
struct debug_read_file_result
{
    uint32 ContentsSize;
    void *Contents;
};
internal debug_read_file_result DEBUGPlatformReadEntireFile(char *Filename);
internal void DEBUGPlatformFreeFileMemory(void *Memory);
internal bool32 DEBUGPlatformWriteEntireFile(char *Filename, uint32 MemorySize, void *Memory);
#endif // HANDMADE_INTERNAL

/**
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
    bool32 IsConnected;
    bool32 IsAnalog;
    real32 StickAverageX;
    real32 StickAverageY;

    union
    {
        game_button_state Buttons[12];
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

            game_button_state Back;
            game_button_state Start;

            /// NOTE(Dennis): NO MORE BUTTONS PAST THIS POINT, EXCEPT THE TERMINATOR!!!

            game_button_state Terminator;
        };
    };
};

struct game_input
{
    /// TODO(Dennis): Insert clock values here.
    game_controller_input Controllers[5];
};
inline game_controller_input *GetController(game_input *Input, int unsigned ControllerIndex)
{
    Assert(ControllerIndex < ArrayCount(Input->Controllers));

    game_controller_input *Result = &Input->Controllers[ControllerIndex];
    return(Result);
}

struct game_memory
{
    bool32 IsInitialized;

    uint64 PermanentStorageSize;
    void *PermanentStorage; /// NOTE(Dennis): REQUIRED to be cleared to zero at startup

    uint64 TransientStorageSize;
    void *TransientStorage; /// NOTE(Dennis): REQUIRED to be cleared to zero at startup
};

internal void GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer);

/// NOTE(Dennis): At the moment, this has to be a very fast function, it cannot be
/// more than a millisecond or so.
/// TODO(Dennis): Reduce the pressure on this function's performance by measuring it
/// or asking about it, etc.
internal void GameGetSoundSamples(game_memory *Memory, game_sound_output_buffer *SoundBuffer);

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
