#include <windows.h>
#include <stdint.h>
#include <xinput.h>

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

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

// TODO(Dennis): This is a global for now.
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;

//NOTE(Dennis): XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputSetState XInputSetState_

//NOTE(Dennis): XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputGetState XInputGetState_

internal void
Win32LoadXInput(void)
{
    //TODO(Dennis): Test this on Windows 8, 7, Vista, XP, 2000? to see which one has which.
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if(!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }

    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
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
RenderWeirdGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset)
{

    uint8 *Row = (uint8 *)Buffer->Memory;

	for(int Y=0;
        Y < Buffer->Height;
        Y++)
	{
		uint32 *Pixel = (uint32 *) Row;
		for(int X=0;
		    X < Buffer->Width;
		    X++)
		{
			/*
			Little Endian
			Pixel in Memory: BB GG RR xx
			0x xxRRGGBB
			*/
			uint8 Blue = (X + XOffset);
			uint8 Green = (Y + YOffset);

			*Pixel++ = ((Green << 8) | Blue);
		}

		Row = Row + Buffer->Pitch;
	}
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    // TODO(Dennis): Bulletproof this.
    // Maybe don't free first, free after, then free first if that fails.

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
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

	Buffer->Pitch = Width*Buffer->BytesPerPixel;

	// TODO(Dennis): Probably clear this to black
}

internal void
Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer,
                           HDC DeviceContext, int WindowWidth, int WindowHeight)
{
    //TODO(Dennis): Aspect ratio correction
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
            // TODO(Dennis): Handle this with a message to the user?
            GlobalRunning = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY:
        {
            // TODO(Dennis): Handle this as an error - recreate window?
            GlobalRunning = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = WParam;
            bool WasDown = ((LParam & (1 << 30)) != 0);
            bool IsDown = ((LParam & (1 << 31)) == 0);
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

                    int msgboxID = MessageBoxA(Window, "Do you really want to quit?", "Handmade Hero", MB_YESNO|MB_ICONWARNING|MB_DEFBUTTON2);
                    switch (msgboxID)
                    {
                        case IDYES:
                        {
                            GlobalRunning = false;
                        }
                    }
                }
                else if(VKCode == VK_SPACE)
                {

                }

            }

            bool AltKeyWasDown = (LParam & (1 << 29) != 0);
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
int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
  Win32LoadXInput();

  WNDCLASSA WindowClass = {};

  Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);

  WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
  WindowClass.lpfnWndProc = Win32MainWindowCallback;
  WindowClass.hInstance = Instance;
//  WindowClass.hIcon;
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

          int XOffset = 0;
          int YOffset = 0;

          GlobalRunning = true;
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

              //TODO(Dennis): Should we poll this more frequently
              for(DWORD ControllerIndex = 0;
                  ControllerIndex < XUSER_MAX_COUNT;
                  ++ControllerIndex)
              {
                  XINPUT_STATE ControllerState;
                  if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                  {
                      //NOTE(Dennis): This controller is plugged in
                      //TODO(Dennis): See if ControllerState.dwPacketNumber increments too rapidly
                      XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

                      bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                      bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                      bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                      bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                      bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
                      bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
                      bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                      bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                      bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
                      bool BButton = (Pad->wButtons & XINPUT_GAMEPAD_B);
                      bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
                      bool YButton = (Pad->wButtons & XINPUT_GAMEPAD_Y);

                      int16 StickX = Pad->sThumbLX;
                      int16 StickY = Pad->sThumbLY;

                      if(AButton)
                      {
                          YOffset += 2;
                      }
                  }
                  else
                  {
                      //NOTE(Dennis): The controller is not available
                      YOffset += 2;
                  }
              }

              RenderWeirdGradient(&GlobalBackbuffer, XOffset, YOffset);

              win32_window_dimensions Dimension = Win32GetWindowDimension(Window);
              Win32DisplayBufferInWindow(&GlobalBackbuffer, DeviceContext,
                                         Dimension.Width, Dimension.Height);

              ++XOffset;
          }
      }
      else
      {
          // TODO(Dennis): Logging
      }
  }
  else
  {
      // TODO(Dennis): Logging
  }

    return(0);
}

