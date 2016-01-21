#include <stdint.h> // defines precise compiler-independent sizes of primitive types

#define internal        static
#define local_persist   static
#define global_variable static

#define Pi32 3.141592665359f

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32   bool32;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float  real32;
typedef double real64;

// NOTE(Joey): utilize a unity build approach 
// (I personally dislike the approach and support a more modular
//  linkage build generating an array of link object files, with
//  the notable requirement of each .cpp and .h file being 
//  largely self-contained s.t. the build configuration only 
//  renders small number of files which keeps things as fast and
//  probably faster in the end, while keeping the code modular)
//  - I still take the approach seeing as it is an interesting
//    educational oppertunity regarding build/compiler configs.
#include "voidt.cpp"

#include <windows.h>
#include <xinput.h> // xbox 360 controller
#include <dsound.h> // DirectSound
#include <math.h>
#include <stdio.h> // c standard runtime/library

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

// globals
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackBuffer;
global_variable LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;


// XInputGetState (dynamic DLL binding without linking)
// this ensures we can use the XInput functions without requiring the DLL during execution
// we declared a stub version of each function in case we can't find any functions
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

// XInputSetState (dynamic DLL binding without linking)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

// DirectSoundCreate
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);




// tries to find XInput function definition from DLL if present
internal void Win32LoadXInput()
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if(!XInputLibrary)
    {
        // TODO(Joey): Diagnostic
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }
    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
        
        // TODO(Joey): Diagnostic
    }
    else
    {
        // TODO(Joey): Diagnostic
    }
}

internal void Win32InitDSound(HWND window, int32 samplesPerSecond, int32 bufferSize)
{
    // load the library
    HMODULE dSoundLibrary = LoadLibrary("dsound.dll");
    if(dSoundLibrary)
    {
         // get a DirectSound object
        direct_sound_create *directSoundCreate = (direct_sound_create*)GetProcAddress(dSoundLibrary, "DirectSoundCreate");
        LPDIRECTSOUND directSound;
        if(directSoundCreate && SUCCEEDED(directSoundCreate(0, &directSound, 0)))
        {
            WAVEFORMATEX waveFormat    = {};
            waveFormat.wFormatTag      = WAVE_FORMAT_PCM;
            waveFormat.nChannels       = 2;
            waveFormat.nSamplesPerSec  = samplesPerSecond;
            waveFormat.wBitsPerSample  = 16;
            waveFormat.nBlockAlign     = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
            waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
            waveFormat.cbSize          = 0;         
        
            // we can call other functions of the loaded directSound object using the (manual) DirectSoundCreate 
            // function pointer instead of loading these functions as well, because the other functions are
            // defined as virutal functions and thus have their memory address stored in the vtable; in which
            // case we know their addresses from the directSound object itself; this is why it works.
            if(!SUCCEEDED(directSound->SetCooperativeLevel(window, DSSCL_PRIORITY)))
            {
                // TODO(Joey): diagnostic
                return;
            }
            // "create" a primary (ring) buffer
            DSBUFFERDESC bufferDescription = { };
            bufferDescription.dwSize  = sizeof(bufferDescription);
            bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
            
            LPDIRECTSOUNDBUFFER primaryBuffer;
            if(SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &primaryBuffer, 0)))
            {   
                
                if(!SUCCEEDED(primaryBuffer->SetFormat(&waveFormat)))
                {
                    // TODO(Joey): diagnostics
                    return;
                }
            }        
            else
            {
                // TODO(Joey): diagnostics
                return;
            }
            
            // "create" a secondary buffer

            bufferDescription = {};
            bufferDescription.dwSize  = sizeof(bufferDescription);
            bufferDescription.dwBufferBytes = bufferSize;
            bufferDescription.lpwfxFormat = &waveFormat;
            
            if(SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &GlobalSecondaryBuffer, 0)))
            {      
                if(!SUCCEEDED(GlobalSecondaryBuffer->SetFormat(&waveFormat)))
                {
                    // TODO(Joey): diagnostics
                    return;
                }
            }               
        }
        else
        {
            // TODO(Joey): diagnostic
            return;
        }
    }
}

internal win32_window_dimension Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension result;
    
    RECT clientRect;
    GetClientRect(Window, &clientRect);
    result.Width = clientRect.right - clientRect.left;
    result.Height = clientRect.bottom - clientRect.top;
    
    return result;
}

internal void RenderWeirdGradiant(win32_offscreen_buffer *Buffer, int XOffset, int YOffset)
{    
    
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

internal void Win32DisplayBufferInWindow(HDC device, win32_offscreen_buffer *Buffer, uint16 WindowWidth, uint16 WindowHeight)
{    
	// copies from one rectangle to the other, possibly stretching
	StretchDIBits(device, 
        0, 0, WindowWidth, WindowHeight,
        0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory, 
		&Buffer->Info,
        DIB_RGB_COLORS, 
        SRCCOPY
    );
            
    
}

LRESULT Win32MainWindowCallBack(
	HWND Window,
	UINT msg,
	WPARAM WParam,
	LPARAM LParam)
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
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = WParam;
            bool WasDown = (LParam & (1 << 30)) != 0;
            bool IsDown = (LParam & (1 << 31)) == 0;
            
            if(WasDown != IsDown)
            {
                if(VKCode == 'W')
                {
                    OutputDebugStringA("W\n");
                }
                else if (VKCode == 'A')
                {
                    
                }
                else if (VKCode == 'S')
                {
                    
                }
                else if (VKCode == 'D')
                {
                    
                }
                else if (VKCode == 'Q')
                {
                    
                }
                else if (VKCode == 'E')
                {
                    
                }
                else if (VKCode == VK_UP)
                {
                    
                }
                else if (VKCode == VK_LEFT)
                {
                    
                }
                else if (VKCode == VK_DOWN)
                {
                    
                }
                else if (VKCode == VK_RIGHT)
                {
                    
                }
                else if (VKCode == VK_ESCAPE)
                {
                    if(IsDown)
                    {
                        GlobalRunning = false;
                    }
                }
                else if (VKCode == VK_SPACE)
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
			int x = Paint.rcPaint.left;
			int y = Paint.rcPaint.top;
			int height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int width = Paint.rcPaint.right - Paint.rcPaint.left;
            
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            
			Win32DisplayBufferInWindow(DeviceContext, &GlobalBackBuffer, Dimension.Width, Dimension.Height);
			EndPaint(Window, &Paint);
		} break;
		default:
		{
			result = DefWindowProc(Window, msg, WParam, LParam);
		} break;	
	}		
	
	return result;
}

struct win32_sound_output
{
    uint32 runningSampleIndex;
    int    samplesPerSecond;     
    int    toneHz;     
    int    toneVolume;
    int    wavePeriod;
    int    bytesPerSample;    
    int    secondaryBufferSize;    
    int    latencySampleCount; // how many samples ahead of the play cursor we'd like to be
    real32 tSine;
};

internal void Win32FillSoundBuffer(win32_sound_output *soundOutput, DWORD byteToLock, DWORD bytesToWrite)
{
    VOID *region1;
    DWORD region1Size;
    VOID *region2;
    DWORD region2Size;                                        
    if(SUCCEEDED(GlobalSecondaryBuffer->Lock(byteToLock, bytesToWrite,
                                             &region1, &region1Size,
                                             &region2, &region2Size,
                                             0)))
    {
        // first buffer region 
        DWORD region1SampleCount = region1Size / soundOutput->bytesPerSample;
        int16 *sample1Out = (int16 *)region1;
        for(DWORD sampleIndex = 0; sampleIndex < region1SampleCount; ++sampleIndex)
        {
            // real32 t = 2.0f * Pi32 * (real32)soundOutput->runningSampleIndex / (real32)soundOutput->wavePeriod;
            real32 sineValue = sinf(soundOutput->tSine);
            int16 sampleValue = (int16)(sineValue * soundOutput->toneVolume);
            // int16 sampleValue = ((runningSampleIndex++ / (wavePeriod / 2)) % 2)  ? toneVolume : - toneVolume;
            *sample1Out++ = sampleValue;
            *sample1Out++ = sampleValue;
            
            soundOutput->tSine += 2.0f * Pi32 * 1.0f / (real32)soundOutput->wavePeriod;
            
            ++soundOutput->runningSampleIndex;
        }
        // second buffer region (if we're over the end of the ring buffer)
        DWORD region2SampleCount = region2Size / soundOutput->bytesPerSample;
        int16 *sample2Out = (int16 *)region2;
        for(DWORD sampleIndex = 0; sampleIndex < region2SampleCount; ++sampleIndex)
        {
            // int16 sampleValue = ((runningSampleIndex++ / (wavePeriod / 2)) % 2) ? toneVolume : - toneVolume;
            // real32 t = 
            real32 sineValue = sinf(soundOutput->tSine);
            int16 sampleValue = (int16)(sineValue * soundOutput->toneVolume);
            *sample2Out++ = sampleValue;
            *sample2Out++ = sampleValue;
            
            soundOutput->tSine += 2.0f * Pi32 * 1.0f / (real32)soundOutput->wavePeriod;
            
             ++soundOutput->runningSampleIndex;
        }         

        GlobalSecondaryBuffer->Unlock(region1, region1Size, region2, region2Size);        
    }
    else
    {
        // NOTE(JOEY): diagnostics
    }
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
	
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
	WindowClass.hInstance = instance;
	WindowClass.lpszClassName = "JoeyWindowClass";
		
	LARGE_INTEGER perfCountFrequency;
    QueryPerformanceFrequency(&perfCountFrequency);
    
    uint64 lastCycleCount = __rdtsc();    
        
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
            HDC DeviceContext = GetDC(Window);
    
            // graphics testbed variables
            int XOffset = 0;
            int YOffset = 0;
            
            // sound testbed variables           
            win32_sound_output soundOutput = {};
                
            soundOutput.samplesPerSecond    = 48000;
            soundOutput.toneHz              = 262; // middle C
            soundOutput.toneVolume          = 3000;
            soundOutput.runningSampleIndex  = 0;
            soundOutput.wavePeriod          = soundOutput.samplesPerSecond / soundOutput.toneHz;
            soundOutput.bytesPerSample      = sizeof(int16) * 2;
            soundOutput.secondaryBufferSize = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;
            soundOutput.latencySampleCount  = soundOutput.samplesPerSecond / 15;

            Win32InitDSound(Window, soundOutput.samplesPerSecond, soundOutput.secondaryBufferSize); 
            Win32FillSoundBuffer(&soundOutput, 0, soundOutput.latencySampleCount * soundOutput.bytesPerSample);
            GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

           
            
            GlobalRunning = true;
            
            LARGE_INTEGER lastCounter;
            QueryPerformanceCounter(&lastCounter);
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
                        
                        if(LeftShoulder)
                        {
                            soundOutput.toneHz -= 1;
                            soundOutput.wavePeriod  = soundOutput.samplesPerSecond / soundOutput.toneHz;
                        }
                        if(RightShoulder)
                        {
                            soundOutput.toneHz += 1;
                            soundOutput.wavePeriod  = soundOutput.samplesPerSecond / soundOutput.toneHz;
                        }
                    }
                    else
                    {
                        // the controller is not available
                    }
                }

                // render
                // RenderWeirdGradiant(&GlobalBackBuffer, XOffset++, YOffset);                
                game_offscreen_buffer buffer = {};
                buffer.Memory = GlobalBackBuffer.Memory;
                buffer.Width = GlobalBackBuffer.Width;
                buffer.Height = GlobalBackBuffer.Height;
                buffer.Pitch = GlobalBackBuffer.Pitch;
                GameUpdateAndRender(&buffer);
                                
                // audio
                DWORD playCursor;
                DWORD writeCursor;
                if(SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&playCursor, &writeCursor)))
                {
                    DWORD byteToLock = (soundOutput.runningSampleIndex * soundOutput.bytesPerSample) % soundOutput.secondaryBufferSize;
                    
                    DWORD targetCursor = (playCursor + soundOutput.latencySampleCount * soundOutput.bytesPerSample) % soundOutput.secondaryBufferSize;
                    DWORD bytesToWrite;
                    if(byteToLock > targetCursor)
                    {
                        bytesToWrite = (soundOutput.secondaryBufferSize - byteToLock); // from byteToLock to end
                        bytesToWrite += targetCursor;                                    // from 0 to position at PlayCursor
                    }
                    else
                    {
                        bytesToWrite = targetCursor - byteToLock;
                    }
                    Win32FillSoundBuffer(&soundOutput, byteToLock, bytesToWrite);
                }
                else
                {
                    // NOTE(JOEY): diagnostics
                }          
                                
                                
                                
                                
                                
                win32_window_dimension Dimension = Win32GetWindowDimension(Window);                
                Win32DisplayBufferInWindow(DeviceContext, &GlobalBackBuffer, Dimension.Width, Dimension.Height);           

                
                uint64 endCycleCount = __rdtsc();
                
                LARGE_INTEGER endCounter;
                QueryPerformanceCounter(&endCounter);
                
                // display values
                uint64 cyclesElapsed  = endCycleCount - lastCycleCount;
                uint64 counterElapsed = endCounter.QuadPart - lastCounter.QuadPart;
                real32 msPerFrame    = ((1000.0f * (real32)counterElapsed) / (real32)perfCountFrequency.QuadPart);
                real32 fps           = (real32)perfCountFrequency.QuadPart / (real32)counterElapsed;
                real32 mcpf          = (real32)(cyclesElapsed / (1000.0f * 1000.0f)); // mega-cycles per frame
                
                char charBuffer[256];
                sprintf(charBuffer, "%.02fms/f / %.02ff/s  -  %.02fmc/f\n", msPerFrame, fps, mcpf); 
                OutputDebugStringA(charBuffer);
                
                lastCounter = endCounter;
                lastCycleCount = endCycleCount;
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
