/// NOTE(Dennis): Finished Day 12, QA is next
/// TODO(Dennis): Capture Debug strings to a file?

/*
   TODO(Dennis): THIS IS NOT A FINAL PLATFORM LAYER!!!

   - Saved game locations
   - Getting a handle to our own executable file
   - Asset loading path
   - Threading (launch a thread)
   - Raw Input (support for multiple keyboards)
   - Sleep/timeBeginPeriod
   - ClipCursor() (multimonitor support)
   - Fullscreen support
   - WM_SETCURSOR (control cursor visibility)
   - QueryCancelAutoplay
   - WM_ACTIVATEAPP (for when we are not the active application)
   - Blit speed improvements (BitBlt)
   - Hardware acceleration (OpenGL or Direct3D or BOTH??)
   - GetKeyboardLayout (for French keyboards, international WASD support)

   Just a partial list of stuff!
*/

/// TODO(Dennis): Implement sine ourselves
#include <math.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f

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

#include "handmade.h"
#include "handmade.cpp"

#include <windows.h>
#include <stdio.h>
#include <xinput.h>
#include <dsound.h>

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

/// TODO(Dennis): This is a global for now.
global_variable bool32 GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;

/// NOTE(Dennis): XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

/// NOTE(Dennis): XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
typedef DIRECT_SOUND_CREATE(direct_sound_create);

void *
PlatformLoadFile(char *FileName)
{
    /// NOTE(Dennis): Implements the Win32 file loading
    return(0);
}

internal void
Win32LoadXInput(void)
{
    /// TODO(Dennis): Test this on Windows 8, 7, Vista, XP to see which one has which.
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if(!XInputLibrary)
    {
        /// TODO(Dennis): Diagnostics
        XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
    }
    if(!XInputLibrary)
    {
        /// TODO(Dennis): Diagnostics
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }

    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        if(!XInputGetState) {XInputGetState = XInputGetStateStub;}

        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
        if(!XInputSetState) {XInputSetState = XInputSetStateStub;}

        /// TODO(Dennis): Diagnostics

    }
    else
    {
        /// TODO(Dennis): Diagnostics
    }
}

internal void
Win32InitDSound(HWND Window, int32 SamplesPerSecond, int32 BufferSize)
{
    /// NOTE(Dennis): Load the library
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if(DSoundLibrary)
    {
        /// NOTE(Dennis): Get an DirectSound object! -cooperative
        direct_sound_create *DirectSoundCreate = (direct_sound_create *)
            GetProcAddress(DSoundLibrary, "DirectSoundCreate");

        WAVEFORMATEX WaveFormat = {};
        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
        WaveFormat.nChannels = 2;
        WaveFormat.nSamplesPerSec = SamplesPerSecond;
        WaveFormat.wBitsPerSample = 16;
        WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
        WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;
        WaveFormat.cbSize = 0;

        /// TODO(Dennis): Double-check that this works on XP - DirectSound 8 or 7??
        LPDIRECTSOUND Directsound;
        if(DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &Directsound, 0)))
        {
            if(SUCCEEDED(Directsound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
            {
                DSBUFFERDESC BufferDescription = {};
                BufferDescription.dwSize = sizeof(BufferDescription);
                BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

                /// NOTE(Dennis): "Create" a primary buffer
                /// TODO(Dennis): DSBCAPS_GLOBALFOCUS?
                LPDIRECTSOUNDBUFFER PrimaryBuffer;
                if(SUCCEEDED(Directsound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
                {
                    HRESULT Error = SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat));
                    if(Error)
                    {
                        /// NOTE(Dennis): We have finally set the format!
                        OutputDebugStringA("Primary buffer format was set.\n");
                    }
                    else
                    {
                        /// TODO(Dennis): Diagnostics
                    }
                }
                else
                {
                    /// TODO(Dennis): Diagnostics
                }
            }
            else
            {
                /// TODO(Dennis): Diagnostics
            }

            /// NOTE(Dennis): "Create" a secondary buffer
            /// TODO(Dennis): DSBCAPS_GETCURRENTPOSITION2?
            DSBUFFERDESC BufferDescription = {};
            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = 0;
            BufferDescription.dwBufferBytes = BufferSize;
            BufferDescription.lpwfxFormat = &WaveFormat;
            HRESULT Error = Directsound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0);
            if(SUCCEEDED(Error))
            {
                OutputDebugStringA("Secondary buffer created successfully.\n");
            }
        }
        else
        {
            /// TODO(Dennis): Diagnostics
        }
    }
    else
    {
        /// TODO(Dennis): Diagnostics
    }
}

internal win32_window_dimensions
Win32GetWindowDimension(HWND Window)
{
    win32_window_dimensions Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return(Result);
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    /// TODO(Dennis): Bulletproof this.
    /// Maybe don't free first, free after, then free first if that fails.

    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	Buffer->Pitch = Width*Buffer->BytesPerPixel;

	/// TODO(Dennis): Probably clear this to black
}

internal void
Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer,
                           HDC DeviceContext, int WindowWidth, int WindowHeight)
{
    /// TODO(Dennis): Aspect ratio correction
    /// TODO(Dennis): Play with stretch modes
    StretchDIBits(DeviceContext,
                  /*
                  X, Y, Width, Height,
                  X, Y, Width, Height,
                  */
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Buffer->Width, Buffer->Height,
                  Buffer->Memory,
                  &Buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{
    LRESULT Result = 0;

    switch(Message)
    {
        case WM_SIZE:
        {
        } break;

        case WM_CLOSE:
        {
            /// TODO(Dennis): Handle this with a message to the user?
            GlobalRunning = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY:
        {
            /// TODO(Dennis): Handle this as an error - recreate window?
            GlobalRunning = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = WParam;
            bool32 WasDown = ((LParam & (1 << 30)) != 0);
            bool32 IsDown = ((LParam & (1 << 31)) == 0);
            if(WasDown != IsDown)
            {
                if(VKCode == 'W')
                {

                }
                else if(VKCode == 'A')
                {

                }
                else if(VKCode == 'S')
                {

                }
                else if(VKCode == 'D')
                {

                }
                else if(VKCode == 'Q')
                {

                }
                else if(VKCode == 'E')
                {

                }
                else if(VKCode == VK_UP)
                {

                }
                else if(VKCode == VK_LEFT)
                {

                }
                else if(VKCode == VK_DOWN)
                {

                }
                else if(VKCode == VK_RIGHT)
                {

                }
                else if(VKCode == VK_ESCAPE)
                {
                    OutputDebugStringA("ESCAPE: ");
                    if(IsDown)
                    {
                        OutputDebugStringA("IsDown ");
                    }
                    if(WasDown)
                    {
                        OutputDebugStringA("WasDown");
                    }
                    OutputDebugStringA("\n");

                    /*int msgboxID = MessageBoxA(Window, "Do you really want to quit?", "Handmade Hero", MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2);
                    switch (msgboxID)
                    {
                        case IDYES:
                        {
                            GlobalRunning = false;
                        }
                    }*/
                }
                else if(VKCode == VK_SPACE)
                {

                }

            }

            bool32 AltKeyWasDown = (LParam & (1 << 29));
            if((VKCode == VK_F4) && AltKeyWasDown)
            {
                GlobalRunning = false;
            }
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;

            win32_window_dimensions Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext,
                                       Dimension.Width, Dimension.Height);
            EndPaint(Window, &Paint);
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }

    return(Result);
}

struct win32_sound_output
{
    /// NOTE(Dennis): Sound test
    int SamplesPerSecond;
    int ToneHz;
    int16 ToneVolume;
    uint32 RunningSampleIndex;
    int WavePeriod;
    int BytesPerSample;
    int SecondaryBufferSize;
    real32 tSine;
    int LatencySampleCount;
};

internal void
Win32ClearBuffer(win32_sound_output *SoundOutput)
{
    VOID *Region1;
    DWORD Region1Size;
    VOID *Region2;
    DWORD Region2Size;
    if(SUCCEEDED(GlobalSecondaryBuffer->Lock(0, SoundOutput->SecondaryBufferSize,
                                             &Region1, &Region1Size,
                                             &Region2, &Region2Size,
                                             0)))
    {
       /// TODO(Dennis): assert that Region1Size/Region2Size is valid
       uint8 *DestSample = (uint8 *)Region1;
       for(DWORD ByteIndex = 0;
           ByteIndex < Region1Size;
           ++ByteIndex)
       {
           *DestSample++ = 0;
       }

       DestSample = (uint8 *)Region2;
       for(DWORD ByteIndex = 0;
           ByteIndex < Region2Size;
           ++ByteIndex)
       {
           *DestSample++ = 0;
       }

       GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
    }
}

internal void
Win32FillSoundBuffer(win32_sound_output *SoundOutput, DWORD ByteToLock, DWORD BytesToWrite,
                     game_sound_output_buffer *SourceBuffer)
{
    /// TODO(Dennis): More strenuous test!
    VOID *Region1;
    DWORD Region1Size;
    VOID *Region2;
    DWORD Region2Size;
    if(SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock, BytesToWrite,
                                            &Region1, &Region1Size,
                                            &Region2, &Region2Size,
                                            0)))
    {
       /// TODO(Dennis): assert that Region1Size/Region2Size is valid

       /// TODO(Dennis): Collapse these two loops
       DWORD Region1SampleCount = Region1Size/SoundOutput->BytesPerSample;
       int16 *DestSample = (int16 *)Region1;
       int16 *SourceSample = SourceBuffer->Samples;
       for(DWORD SampleIndex = 0;
           SampleIndex < Region1SampleCount;
           ++SampleIndex)
       {
           *DestSample++ = *SourceSample++;
           *DestSample++ = *SourceSample++;
           ++SoundOutput->RunningSampleIndex;
       }

       DWORD Region2SampleCount = Region2Size/SoundOutput->BytesPerSample;
       DestSample = (int16 *)Region2;
       for(DWORD SampleIndex = 0;
           SampleIndex < Region2SampleCount;
           ++SampleIndex)
       {
           *DestSample++ = *SourceSample++;
           *DestSample++ = *SourceSample++;
           ++SoundOutput->RunningSampleIndex;
       }

       GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
   }
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
  LARGE_INTEGER PerfCountFrequencyResult;
  QueryPerformanceFrequency(&PerfCountFrequencyResult);
  int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;

  Win32LoadXInput();

  WNDCLASSA WindowClass = {};

  Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

  WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
  ///WindowClass.hIcon;
  WindowClass.lpszClassName = "HandmadeHeroWindowClass";

  if(RegisterClassA(&WindowClass))
  {
      HWND Window =
           CreateWindowExA(
                0,
                WindowClass.lpszClassName,
                "Handmade Hero",
                WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                0,
                0,
                Instance,
                0);
      if(Window)
      {
          HDC DeviceContext = GetDC(Window);

          /// NOTE(Dennis): Graphics test
          int XOffset = 0;
          int YOffset = 0;

          win32_sound_output SoundOutput = {};

          /// TODO(Dennis): Make this like sixty seconds?
          SoundOutput.SamplesPerSecond = 48000;
          SoundOutput.ToneHz = 256;
          SoundOutput.ToneVolume = 6000;
          SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond/SoundOutput.ToneHz;
          SoundOutput.BytesPerSample = sizeof(int16)*2;
          SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond*SoundOutput.BytesPerSample;
          SoundOutput.LatencySampleCount = SoundOutput.SamplesPerSecond / 15;
          Win32InitDSound(Window, SoundOutput.SamplesPerSecond, SoundOutput.SecondaryBufferSize);
          Win32ClearBuffer(&SoundOutput);
          GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

          GlobalRunning = true;

          /// TODO(Dennis): Pool with bitmap VirtualAlloc
          int16 *Samples = (int16 *)VirtualAlloc(0, SoundOutput.SecondaryBufferSize,
                                                 MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

          LARGE_INTEGER LastCounter;
          QueryPerformanceCounter(&LastCounter);

          uint64 LastCycleCount = __rdtsc();
          while(GlobalRunning)
          {
              MSG Message;

              while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
              {
                  if(Message.message == WM_QUIT)
                  {
                    GlobalRunning = false;
                  }

                  TranslateMessage(&Message);
                  DispatchMessageA(&Message);
              }

              /// TODO(Dennis): Should we poll this more frequently
              for(DWORD ControllerIndex = 0;
                  ControllerIndex < XUSER_MAX_COUNT;
                  ++ControllerIndex)
              {
                  XINPUT_STATE ControllerState;
                  if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                  {
                      /// NOTE(Dennis): This controller is plugged in
                      /// TODO(Dennis): See if ControllerState.dwPacketNumber increments too rapidly
                      XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

                      bool32 Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                      bool32 Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                      bool32 Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                      bool32 Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                      bool32 Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
                      bool32 Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
                      bool32 LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                      bool32 RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                      bool32 AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
                      bool32 BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                      bool32 XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
                      bool32 YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

                      int16 StickX = Pad->sThumbLX;
                      int16 StickY = Pad->sThumbLY;

                      /// TODO(Dennis): We will do proper deadzone handling later using
                      /// XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
                      /// XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE

                      XOffset += StickX / 4096;
                      YOffset += StickY / 4096;

                      SoundOutput.ToneHz = 512 + (int)(256.0f*((real32)StickY / 30000.0f));
                      SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond/SoundOutput.ToneHz;
                  }
                  else
                  {
                      /// NOTE(Dennis): The controller is not available
                      XOffset += 2;
                      YOffset += 2;
                  }
              }

              DWORD ByteToLock;
              DWORD TargetCursor;
              DWORD BytesToWrite;
              DWORD PlayCursor;
              DWORD WriteCursor;
              bool32 SoundIsValid = false;
              /// TODO(Dennis): Tighten up sound logic so that we know where we should be
              /// writing to and can anticipate the time spent in the game update.
              if(SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
              {
                  ByteToLock = ((SoundOutput.RunningSampleIndex*SoundOutput.BytesPerSample) %
                                      SoundOutput.SecondaryBufferSize);

                  TargetCursor =
                         ((PlayCursor +
                           (SoundOutput.LatencySampleCount*SoundOutput.BytesPerSample)) %
                          SoundOutput.SecondaryBufferSize);
                  if(ByteToLock > TargetCursor)
                  {
                      BytesToWrite = (SoundOutput.SecondaryBufferSize - ByteToLock);
                      BytesToWrite += TargetCursor;
                  }
                  else
                  {
                      BytesToWrite = TargetCursor - ByteToLock;
                  }

                  SoundIsValid = true;
              }

              game_sound_output_buffer SoundBuffer = {};
              SoundBuffer.SamplesPerSecond = SoundOutput.SamplesPerSecond;
              SoundBuffer.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
              SoundBuffer.Samples = Samples;

              game_offscreen_buffer Buffer = {};
              Buffer.Memory = GlobalBackbuffer.Memory;
              Buffer.Width = GlobalBackbuffer.Width;
              Buffer.Height = GlobalBackbuffer.Height;
              Buffer.Pitch = GlobalBackbuffer.Pitch;
              GameUpdateAndRender(&Buffer, XOffset, YOffset, &SoundBuffer, SoundOutput.ToneHz);

              /// NOTE(Dennis): DirectSound output test
              if(SoundIsValid)
              {
                Win32FillSoundBuffer(&SoundOutput, ByteToLock, BytesToWrite, &SoundBuffer);
              }

             win32_window_dimensions Dimension = Win32GetWindowDimension(Window);
             Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext,
                                        Dimension.Width, Dimension.Height);

             uint64 EndCycleCount = __rdtsc();

             LARGE_INTEGER EndCounter;
             QueryPerformanceCounter(&EndCounter);

             uint64 CyclesElapsed = EndCycleCount - LastCycleCount;
             int64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
             real64 MSPerFrame = (((1000.0f*(real64)CounterElapsed) / (real64)PerfCountFrequency));
             real64 FPS = (real64)PerfCountFrequency / (real64)CounterElapsed;
             real64 MCPF = ((real64)CyclesElapsed / (1000.0f * 1000.0f));

#if HANDMADE_INTERNAL
             char TextBuffer[256];
             sprintf(TextBuffer, "%.02fms/f, %.02ff/s, %.02fmc/f\n", MSPerFrame, FPS, MCPF);
             OutputDebugStringA(TextBuffer);
#endif // HANDMADE_INTERNAL
             LastCounter = EndCounter;
             LastCycleCount = EndCycleCount;
          }
      }
      else
      {
          /// TODO(Dennis): Logging
      }
  }
  else
  {
      /// TODO(Dennis): Logging
  }

    return(0);
}

