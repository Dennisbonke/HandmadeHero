#if !defined(WIN32_HANDMADE_H)

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Pitch;
    int Width;
    int Height;
    int BytesPerPixel;
};

struct win32_window_dimensions
{
    int Width;
    int Height;
};

struct win32_sound_output
{
    /// NOTE(Dennis): Sound test
    int SamplesPerSecond;
    uint32 RunningSampleIndex;
    int BytesPerSample;
    DWORD SecondaryBufferSize;
    DWORD SafetyBytes;
    real32 tSine;
    int LatencySampleCount;
    /// TODO(Dennis): Should RunningSampleIndex be in bytes as well?
    /// TODO(Dennis): Math gets simpler if we add a "bytes per second" field?
};

struct win32_debug_time_marker
{
    DWORD OutputPlayCursor;
    DWORD OutputWriteCursor;
    DWORD OutputLocation;
    DWORD OutputByteCount;
    DWORD ExpectedFlipPlayCursor;

    DWORD FlipPlayCursor;
    DWORD FlipWriteCursor;
};

#define WIN32_HANDMADE_H
#endif
