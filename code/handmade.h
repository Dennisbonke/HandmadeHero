#ifndef HANDMADE_H
#define HANDMADE_H

/**
   NOTE(Dennis):
HANDMADE_INTERNAL:
  0 - Build for public release
  1 - Build for developer only

HANDMADE_SLOW:
  0 - No slow code allowed!
  1 - Slow code welcome!
*/

/// TODO(Dennis): Implement sine ourselves
#include <math.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f

// NOTE(Dennis): Don't keep this, only cuz sine wave makes you go crazy
#define PLZ_STFU 1

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

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
/// IMPORTANT(Dennis): These are NOT for doing anything in the shipping game - they are blocking and the write does not protect against lost data!
struct debug_read_file_result
{
    uint32 ContentsSize;
    void *Contents;
};

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(void *Memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(char *Filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(char *Filename, uint32 MemorySize, void *Memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

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
    int BytesPerPixel;
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
#if HANDMADE_INTERNAL
    debug_platform_free_file_memory *DEBUGPlatformFreeFileMemory;
    debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
    debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
#endif
};

#define GAME_UPDATE_AND_RENDER(name) void name(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
}

/// NOTE(Dennis): At the moment, this has to be a very fast function, it cannot be
/// more than a millisecond or so.
/// TODO(Dennis): Reduce the pressure on this function's performance by measuring it
/// or asking about it, etc.
#define GAME_GET_SOUND_SAMPLES(name) void name(game_memory *Memory, game_sound_output_buffer *SoundBuffer)
typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples);
GAME_GET_SOUND_SAMPLES(GameGetSoundSamplesStub)
{
}

///
///
///

struct game_state
{
    int ToneHz;
    int GreenOffset;
    int BlueOffset;
    
    real32 tSine;

    int PlayerX;
    int PlayerY;
    real32 tJump;
};

#endif
