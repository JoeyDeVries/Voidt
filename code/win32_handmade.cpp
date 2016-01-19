#include <windows.h>
#include <stdint.h> // defines precise compiler-independent sizes of primitive types
#include <xinput.h> // xbox 360 controller

#define internal        static
#define local_persist   static
#define global_variable static

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct win32_offscreen_buffer
{
    BITMAPINFO  Info;
    void       *Memory;
    uint16      Width;
    uint16      Height;
    uint32      Pitch;
    uint8       BytesPerPixel;
    uint8       Padding[3]; // for 32 byte alignment
};

struct win32_window_dimension
{
    uint16 Width;
    uint16 Height;
};

// XInputGetState (dynamic DLL binding without linking)
// this ensures we can use the XInput functions without requiring the DLL during execution
// we declared a stub version of each function in case we can't find any functions
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return 0;
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;

// XInputSetState (dynamic DLL binding without linking)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return 0;
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_


global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackBuffer;


// tries to find XInput function definition from DLL if present
internal void Win32LoadXInput()
{
    HMODULE XInputLibrary = LoadLibrary("xinput1_3.dll");
    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}

internal win32_window_dimension Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    
    return Result;
}

internal void RenderWeirdGradiant(win32_offscreen_buffer Buffer, int XOffset, int YOffset)
{    
    // int pitch = Buffer.Width * Buffer.BytesPerPixel;
    uint8 *row = (uint8 *)Buffer.Memory; 
    for(int Y = 0; Y < Buffer.Height; ++Y)
    {
        uint32 *Pixel = (uint32 *)row;
        for(int X = 0; X < Buffer.Width; ++X)
        {    
            uint8 Blue  = X + XOffset; // B
            uint8 Green = Y + YOffset; // G
            uint8 Red   = X + YOffset + XOffset;           // R
                        
            *Pixel++ = (Red << 16) | (Green << 8) | Blue;
        }
        row += Buffer.Pitch;
    }    
}


internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int width, int height)
{
    // free previous memory if we're going to resize
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }
    
    Buffer->Width  = width;
    Buffer->Height = height;
    Buffer->BytesPerPixel = 4;
    
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height; // negative biHeight sets bitmap origin to top-left corner
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	// creates a new bitmap to render to (allocate memory ourselves)
    int bitmapMemorySize = (width * height) * Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE); // reserves/allocates complete pages
    
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
}

internal void Win32DisplayBufferInWindow(HDC device, uint16 WindowWidth, uint16 WindowHeight, win32_offscreen_buffer Buffer)
{    
	// copies from one rectangle to the other, possibly stretching
	StretchDIBits(device, 
        0, 0, WindowWidth, WindowHeight,
        0, 0, Buffer.Width, Buffer.Height,
		Buffer.Memory, 
		&Buffer.Info,
        DIB_RGB_COLORS, 
        SRCCOPY
    );
            
    
}

LRESULT Win32MainWindowCallBack(
	HWND Window,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT result = 0;
	
	switch(msg)
	{
		case WM_SIZE:
		{
            // win32_window_dimension Dimension = Win32GetWindowDimension(Window);
			// Win32ResizeDIBSection(&GlobalBackBuffer, Dimension.Width, Dimension.Height);
		} break;
		case WM_CLOSE:
		{
			GlobalRunning = false;
		} break;
		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;
		case WM_DESTROY:
		{
			GlobalRunning = false;
		} break;
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int x = Paint.rcPaint.left;
			int y = Paint.rcPaint.top;
			int height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int width = Paint.rcPaint.right - Paint.rcPaint.left;
            
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            
			Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackBuffer);
			EndPaint(Window, &Paint);
		} break;
		default:
		{
			result = DefWindowProc(Window, msg, wParam, lParam);
		} break;	
	}		
	
	return result;
}

int CALLBACK WinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	LPSTR cmdLine,
	int cmdShow)
{
	// MessageBox(0, "Sup G", "Joey", MB_OK | MB_ICONINFORMATION);	
    Win32LoadXInput();
    
	WNDCLASS WindowClass = {};
    
    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);
	
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
	WindowClass.hInstance = instance;
	WindowClass.lpszClassName = "JoeyWindowClass";
		
	
	if(RegisterClass(&WindowClass))
	{
		HWND Window = CreateWindowEx(
			0,
			WindowClass.lpszClassName,
			"Joey game",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			instance,
			0
		);
		
		if(Window)
		{
			GlobalRunning = true;

            int XOffset = 0;
            int YOffset = 0;
			while(GlobalRunning)
			{
                MSG Message;
                // process all windows messages currently in Queue               
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT)
                        GlobalRunning = false;
                    
                    TranslateMessage(&Message);
					DispatchMessage(&Message);
                }
                
                // poll xbox 360 controller(s)
                for(DWORD i = 0; i < XUSER_MAX_COUNT; ++i)
                {
                    XINPUT_STATE ControllerState;
                    if(XInputGetState(i, &ControllerState) == ERROR_SUCCESS)
                    {
                        // the controller is plugged in
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
                        
                        bool Up            = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool Down          = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left          = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right         = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        bool Start         = (Pad->wButtons & XINPUT_GAMEPAD_START);
                        bool Back          = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
                        bool LeftShoulder  = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool AButton       = (Pad->wButtons & XINPUT_GAMEPAD_A);
                        bool BButton       = (Pad->wButtons & XINPUT_GAMEPAD_B);
                        bool XButton       = (Pad->wButtons & XINPUT_GAMEPAD_X);
                        bool YButton       = (Pad->wButtons & XINPUT_GAMEPAD_Y);
                        
                        int16 StickX = Pad->sThumbLX;
                        int16 StickY = Pad->sThumbLY;
                        
                        if(AButton)
                        {
                            ++YOffset;
                            
                        }
                        if(BButton)
                        {
                            XINPUT_VIBRATION Vibration;
                            Vibration.wLeftMotorSpeed = 65535;
                            Vibration.wRightMotorSpeed = 65535;
                            XInputSetState(i, &Vibration);
                        }
                    }
                    else
                    {
                        // the controller is not available
                    }
                }

                
                RenderWeirdGradiant(GlobalBackBuffer, XOffset++, YOffset);
                HDC DeviceContext = GetDC(Window);
                
                win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                
                Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackBuffer);
                
                ReleaseDC(Window, DeviceContext);                
			}
		}
		else
		{
			OutputDebugStringA("WindowHandle creation FAILED\n");
		}
	}
	else
	{
		OutputDebugStringA("RegisterClass FAILED\n");
	}
	

	return (0);
}
