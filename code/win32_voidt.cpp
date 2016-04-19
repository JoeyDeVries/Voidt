/*******************************************************************
** Copyright (C) 2015-2016 {Joey de Vries} {joey.d.vries@gmail.com}
**
** This code is part of Voidt.
** https://github.com/JoeyDeVries/Voidt
**
** Voidt is free software: you can redistribute it and/or modify it
** under the terms of the CC BY-NC 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
*******************************************************************/

/* NOTE(Joey): 
    
  Utilize a unity build approach.
   
  (I personally dislike the approach and support a more modular
  linkage build generating an array of link object files, with
  the notable requirement of each .cpp and .h file being 
  largely self-contained s.t. the build configuration only 
  renders small number of files which keeps things as fast and
  probably faster in the end, while keeping the code modular).
   
  I still take the approach seeing as it is an interesting
  educational oppertunity regarding build/compiler configs.
*/
#include "voidt.h"

#include <windows.h>
#include <xinput.h> // xbox 360 controller
#include <dsound.h> // DirectSound
#include <stdio.h> // c standard runtime/library
#include <stdarg.h> // va_args 

#include "win32_voidt.h"



internal void CatStrings(size_t sourceACount, char *sourceA, size_t sourceBCount, char *sourceB, size_t destCount, char *dest)
{
    for(int i = 0; i < sourceACount; ++i)
    {
        *dest++ = *sourceA++;
    }
    for(int i = 0; i < sourceBCount; ++i)
    {
        *dest++ = *sourceB++;
    }
    *dest++ = 0;
}    

internal int StringLength(char *string)
{
    int count = 0;
    while(*string++)
        ++count;
    return count;    
}

internal void Win32GetEXEFileName(win32_state *state)
{
    DWORD sizeOfFilename = GetModuleFileNameA(0, state->EXEFileName, sizeof(state->EXEFileName));
    state->OnePastLastEXEFileNameSlash = state->EXEFileName;
    for(char *scan = state->EXEFileName; *scan; ++scan)
    {
        if(*scan == '\\')
            state->OnePastLastEXEFileNameSlash = scan + 1;
    }
}

internal void Win32BuildEXEPathFileName(win32_state *state, char *fileName, int destCount, char *dest)
{
    CatStrings(state->OnePastLastEXEFileNameSlash - state->EXEFileName, state->EXEFileName,
               StringLength(fileName), fileName, 
               destCount, dest);
}
      

inline uint32 SafeTruncateUInt64(uint64 value)
{
    Assert(value < 0xFFFFFFFF); // make sure size is less than 32 bits 4GB (or we have to loop for larger files)
    uint32 result = (uint32)value;
    return result;
}

DEBUG_PLATFORM_FREE_FILE_MEMORY(DEBUGPlatformFreeFileMemory)
{
    if(memory)
    {
        VirtualFree(memory, 0, MEM_RELEASE);
        memory = 0;
    }
}

DEBUG_PLATFORM_READ_ENTIRE_FILE(DEBUGPlatformReadEntireFile)
{
    debug_read_file_result result = {};
    
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if(GetFileSizeEx(fileHandle, &fileSize))
        {
            // TODO(Joey): defines for maximum values
            uint32 fileSize32 = SafeTruncateUInt64(fileSize.QuadPart);
            result.Contents = VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if(result.Contents)
            {
                DWORD bytesRead;
                if(ReadFile(fileHandle, result.Contents, fileSize32, &bytesRead, 0) && fileSize32 == bytesRead)
                {
                    result.ContentSize = fileSize32;
                }
                else
                {
                    DEBUGPlatformFreeFileMemory(result.Contents);
                    result.ContentSize = 0;
                }
            }
        }        
        CloseHandle(fileHandle);
    }
    return result;
}

DEBUG_PLATFORM_WRITE_ENTIRE_FILE(DEBUGPlatformWriteEntireFile)
{
    bool32 result = false;
    
    HANDLE fileHandle = CreateFileA(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if(fileHandle != INVALID_HANDLE_VALUE)
    {            
        DWORD bytesWritten;
        if(WriteFile(fileHandle, memory, memorySize, &bytesWritten, 0))
        {
            result = (bytesWritten == memorySize);
        }
        else
        {
            // TODO(Joey): diagnostics
        }

        CloseHandle(fileHandle);
    }
    return result;
}

PLATFORM_WRITE_DEBUG_OUTPUT(Win32WriteDebugOutput)
{
    char buffer[256];
    va_list argptr;
    va_start(argptr, format);
    vsnprintf(buffer, sizeof(buffer), format, argptr);
    OutputDebugStringA(buffer);
    va_end(argptr);
}

inline FILETIME Win32GetLastWriteTime(char *fileName)
{
    FILETIME lastWriteTime = {};
    
    WIN32_FILE_ATTRIBUTE_DATA data;
    if(GetFileAttributesEx(fileName, GetFileExInfoStandard, &data))
        lastWriteTime = data.ftLastWriteTime;
    
    return lastWriteTime;
}

internal win32_game_code Win32LoadGameCode(char *sourceDLLName, char *tempDLLName, char *lockFileName)
{
    win32_game_code result = {};
    
    WIN32_FILE_ATTRIBUTE_DATA ignored;
    if(!GetFileAttributesEx(lockFileName, GetFileExInfoStandard, &ignored))
    {
        result.DLLLastWriteTime = Win32GetLastWriteTime(sourceDLLName);
          
        CopyFile(sourceDLLName, tempDLLName, FALSE);
        result.GameCodeDLL = LoadLibraryA(tempDLLName);
        if(result.GameCodeDLL)
        {
            result.UpdateAndRender = (game_update_and_render*)GetProcAddress(result.GameCodeDLL, "GameUpdateAndRender");
            result.GetSoundSamples = (game_get_sound_samples*)GetProcAddress(result.GameCodeDLL, "GameGetSoundSamples");
            
            result.IsValid = result.UpdateAndRender && result.GetSoundSamples;
        }
    }
    
    if(!result.IsValid)
    {
        result.UpdateAndRender = GameUpdateAndRenderStub;
        result.GetSoundSamples = GameGetSoundSamplesStub;    
    }
    
    return result;
}

internal void Win32UnloadGameCode(win32_game_code *gameCode)
{
    if(gameCode->GameCodeDLL)        
    {
        FreeLibrary(gameCode->GameCodeDLL);
        gameCode->GameCodeDLL = 0;
    }
    

    gameCode->IsValid = false;
    gameCode->UpdateAndRender = GameUpdateAndRenderStub;
    gameCode->GetSoundSamples = GameGetSoundSamplesStub;
}

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
            bufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_GLOBALFOCUS;
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

internal void ToggleFullScreen(HWND window)
{
    DWORD style = GetWindowLong(window, GWL_STYLE);
    if(style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitorInfo = { sizeof(monitorInfo) };
        if(GetWindowPlacement(window, &GlobalWindowPosition) && GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
        {
            SetWindowLong(window, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(window, HWND_TOP, 
                         monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
                         monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                         monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            
        }        
    }
    else
    {
        SetWindowLong(window, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(window, &GlobalWindowPosition);
        SetWindowPos(window, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int width, int height)
{
    // free previous memory if we're going to resize
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }
    
    Buffer->Width         = width;
    Buffer->Height        = height;
    Buffer->BytesPerPixel = 4;
    
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = Buffer->Height; // negative biHeight sets bitmap origin to top-left corner; default bottom-left
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	// creates a new bitmap to render to (allocate memory ourselves)
    int bitmapMemorySize = (width * height) * Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE); // reserves/allocates complete pages
    
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
}

internal void Win32DisplayBufferInWindow(HDC device, win32_offscreen_buffer *Buffer, int32 WindowWidth, int32 WindowHeight)
{    
    // PatBlt(device, 0, 0, WindowWidth, WindowHeight, BLACKNESS);    
    if(WindowWidth >= Buffer->Width*2 && WindowHeight >= Buffer->Height*2)
    {
         StretchDIBits(device, 
            0, 0, WindowWidth, WindowHeight,
            0, 0, Buffer->Width, Buffer->Height,
            Buffer->Memory, 
            &Buffer->Info,
            DIB_RGB_COLORS, 
            SRCCOPY
        );       
    }
    else
    {        
        int offsetX = 0;
        int offsetY = 0;

        // copies from one rectangle to the other, possibly stretching
        StretchDIBits(device, 
            offsetX, offsetY, Buffer->Width, Buffer->Height,
            0, 0, Buffer->Width, Buffer->Height,
            Buffer->Memory, 
            &Buffer->Info,
            DIB_RGB_COLORS, 
            SRCCOPY
        );
    }  
    
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
		// case WM_SIZE:
		// {
            // win32_window_dimension Dimension = Win32GetWindowDimension(Window);
			// Win32ResizeDIBSection(&GlobalBackBuffer, Dimension.Width, Dimension.Height);
		// } break;
		case WM_CLOSE:
		{
			GlobalRunning = false;
		} break;
        case WM_SETCURSOR:
        {
            if(DEBUGGlobalShowCursor)
            {
                result = DefWindowProc(Window, msg, WParam, LParam);
            }
            else
                SetCursor(0);
            break;
        }
		case WM_ACTIVATEAPP:
		{
            if(WParam == TRUE)
                SetLayeredWindowAttributes(Window, RGB(0, 0, 0), (BYTE)255, LWA_ALPHA);
            else
                SetLayeredWindowAttributes(Window, RGB(0, 0, 0), (BYTE)64, LWA_ALPHA);
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
            Assert(!"Keyboard input came through non-dispatch message!");                      
            
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


internal void Win32ClearBuffer(win32_sound_output *soundOutput)
{
    VOID *region1;
    DWORD region1Size;
    VOID *region2;
    DWORD region2Size;     
    if(SUCCEEDED(GlobalSecondaryBuffer->Lock(0, soundOutput->secondaryBufferSize,
                                             &region1, &region1Size,
                                             &region2, &region2Size,
                                             0)))
    {
        uint8 *destSample = (uint8 *)region1;
        for(DWORD byteIndex = 0; byteIndex < region1Size; ++byteIndex)
           *destSample++ = 0;
        destSample = (uint8 *)region2;
        for(DWORD byteIndex = 0; byteIndex < region2Size; ++byteIndex)
           *destSample++ = 0;
       
       GlobalSecondaryBuffer->Unlock(region1, region1Size, region2, region2Size);        
    }
}

internal void Win32FillSoundBuffer(win32_sound_output *soundOutput, DWORD byteToLock, DWORD bytesToWrite, game_sound_output_buffer *sourceBuffer)
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
        int16 *destSample = (int16 *)region1;
        int16 *sourceSample = sourceBuffer->Samples;
        for(DWORD sampleIndex = 0; sampleIndex < region1SampleCount; ++sampleIndex)
        {
            *destSample++ = *sourceSample++;
            *destSample++ = *sourceSample++;
            ++soundOutput->runningSampleIndex;
        }
        // second buffer region (if we're over the end of the ring buffer)
        DWORD region2SampleCount = region2Size / soundOutput->bytesPerSample;
        destSample = (int16 *)region2;
        for(DWORD sampleIndex = 0; sampleIndex < region2SampleCount; ++sampleIndex)
        {
            *destSample++ = *sourceSample++;
            *destSample++ = *sourceSample++;
             ++soundOutput->runningSampleIndex;
        }         

        GlobalSecondaryBuffer->Unlock(region1, region1Size, region2, region2Size);        
    }
    else
    {
        // NOTE(JOEY): diagnostics
    }
}

internal void Win32ProcessXInputDigitalButton(DWORD xInputButtonState, game_button_state *oldState, DWORD buttonBit, game_button_state *newState)
{
    newState->EndedDown = ((xInputButtonState & buttonBit) == buttonBit);
    newState->HalfTransitionCount = (oldState->EndedDown != newState->EndedDown) ? 1 : 0;
}

internal void Win32ProcessKeyboardMessage(game_button_state *newState, bool32 isDown)
{
    if(newState->EndedDown != isDown)
    {
        newState->EndedDown = isDown;
        ++newState->HalfTransitionCount;
    }
}

inline LARGE_INTEGER Win32GetWallClock()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter;
}

inline real32 Win32GetSecondsElapsed(LARGE_INTEGER start, LARGE_INTEGER end)
{
    real32 result  = (real32)(end.QuadPart - start.QuadPart) / (real32)GlobalPerfCountFrequency.QuadPart;
    return result;
}
                

internal void Win32GetInputFileLocation(win32_state *state, int slotIndex, int destCount, char *dest)
{
    char temp[MAX_PATH];
    wsprintf(temp, "inp_rcd%d.vri", slotIndex);
    Win32BuildEXEPathFileName(state, temp, destCount, dest);
}    

// copy memory from A to B given a size
internal void Win32CopyMemory(void *from, void *to,	uint64 size)
{
	// we copy in 64bit chunks; which is substantially faster then 8bit chunks
	// as in: 1gibibyte = 10s 8bit copy vs barely 1s with 64bit copy
	// this can be even more rediculasly faster with SIMD and some assembly
	uint64 size64bit = size / 8;
	uint64 *src = (uint64 *)from;
	uint64 *cpy = (uint64 *)to;
	for (uint64 i = 0; i < size64bit; ++i) {
		*cpy++ = *src++;
	}

	// copy whatever's left
	uint64 size8bit = size - (size64bit * 8);
	uint8 *src8 = (uint8 *)src;
	uint8 *cpy8 = (uint8 *)cpy;
	for (uint64 i = 0; i < size8bit; ++i) {
		*cpy8++ = *src8++;
	}
}
                   
internal void Win32BeginRecordingInput(win32_state *state, int inputRecordingIndex)
{
    Assert(inputRecordingIndex < ArrayCount(state->ReplayBuffers));
    win32_replay_buffer *recordBuffer = &state->ReplayBuffers[inputRecordingIndex];
    if(recordBuffer->MemoryBlock)
    {
        state->InputRecordingIndex = inputRecordingIndex;    
        
        char fileName[MAX_PATH];
        Win32GetInputFileLocation(state, inputRecordingIndex, sizeof(fileName) - 1, fileName);
        
        state->RecordingHandle = CreateFile(fileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
        // DWORD ignored;
        // DeviceIoControl(state->RecordingHandle, FSCTL_SET_SPARSE, 0, 0, 0, 0, &ignored, 0);
        
        // Win32CopyMemory(state->GameMemoryBlock, recordBuffer->MemoryBlock, state->TotalSize); // possibly faster due to 64 bit writes
        CopyMemory(recordBuffer->MemoryBlock, state->GameMemoryBlock, state->TotalSize);
        // DWORD bytesToWrite = (DWORD)state->TotalSize;
        // Assert(state->TotalSize == bytesToWrite);
        // DWORD bytesWritten;
        // WriteFile(state->RecordingHandle, state->GameMemoryBlock, bytesToWrite, &bytesWritten, 0);
    }
}

internal void Win32EndRecordingInput(win32_state *win32State)
{
    CloseHandle(win32State->RecordingHandle);
    win32State->InputRecordingIndex = 0;
}

internal void Win32BeginInputPlayback(win32_state *state, int inputPlayingIndex)
{
    Assert(inputPlayingIndex < ArrayCount(state->ReplayBuffers));
    win32_replay_buffer *recordBuffer = &state->ReplayBuffers[inputPlayingIndex];
    if(recordBuffer->MemoryBlock)
    {
        state->InputPlayingIndex = inputPlayingIndex;
        
        char fileName[MAX_PATH];
        Win32GetInputFileLocation(state, inputPlayingIndex, sizeof(fileName) - 1, fileName);
        
        state->PlaybackHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        
        
        // Win32CopyMemory(recordBuffer->MemoryBlock, state->GameMemoryBlock, state->TotalSize);    
        CopyMemory(state->GameMemoryBlock, recordBuffer->MemoryBlock, state->TotalSize);
        // DWORD bytesToRead = (DWORD)state->TotalSize;
        // Assert(state->TotalSize == bytesToRead);
        // DWORD bytesRead;
        // ReadFile(state->PlaybackHandle, state->GameMemoryBlock, bytesToRead, &bytesRead, 0);
    }
}

internal void Win32EndInputPlayback(win32_state *win32State)
{
    CloseHandle(win32State->PlaybackHandle);
    win32State->InputPlayingIndex = 0;
}

// write input to file
internal void Win32RecordInput(win32_state *win32State, game_input *newInput)
{
    DWORD bytesWritten;
    WriteFile(win32State->RecordingHandle, newInput, sizeof(*newInput), &bytesWritten, 0);
}

internal void Win32PlaybackInput(win32_state *win32State, game_input *newInput)
{
    DWORD bytesRead = 0;
    if(ReadFile(win32State->PlaybackHandle, newInput, sizeof(*newInput), &bytesRead, 0))
    {
        if(bytesRead == 0)
        {
            int playingIndex = win32State->InputPlayingIndex;
            Win32EndInputPlayback(win32State);
            Win32BeginInputPlayback(win32State, playingIndex);
            ReadFile(win32State->PlaybackHandle, newInput, sizeof(*newInput), &bytesRead, 0);
        }
    }
}
      
internal void Win32ProcessPendingMessages(win32_state *win32State, game_controller_input *keyboardController)
{    
    MSG Message;
    // process all windows messages currently in Queue               
    while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {        
        switch(Message.message)
        {
            case WM_QUIT:
                GlobalRunning = false;
            break;
            
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                uint32 VKCode = (uint32)Message.wParam;
                bool WasDown = (Message.lParam & (1 << 30)) != 0;
                bool IsDown = (Message.lParam & (1 << 31)) == 0;
                
                if(WasDown != IsDown)
                {
                    if(VKCode == 'W')
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->MoveUp, IsDown);
                    }
                    else if (VKCode == 'A')
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->MoveLeft, IsDown);
                    }
                    else if (VKCode == 'S')
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->MoveDown, IsDown);
                    }
                    else if (VKCode == 'D')
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->MoveRight, IsDown);
                    }
                    else if (VKCode == 'Q')
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->LeftShoulder, IsDown);
                    }
                    else if (VKCode == 'E')
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->LeftShoulder, IsDown);
                    }
                    else if (VKCode == VK_UP)
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->ActionUp, IsDown);
                    }
                    else if (VKCode == VK_LEFT)
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->ActionLeft, IsDown);
                    }
                    else if (VKCode == VK_DOWN)
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->ActionDown, IsDown);
                    }
                    else if (VKCode == VK_RIGHT)
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->ActionRight, IsDown);
                    }
                    else if (VKCode == VK_ESCAPE)
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->Back, IsDown);
                    }
                    else if (VKCode == VK_SPACE)
                    {
                        Win32ProcessKeyboardMessage(&keyboardController->Start, IsDown);
                    }             
                    else if (VKCode == 'L')
                    {
                        if(IsDown)
                        {
                            if(win32State->InputPlayingIndex == 0)
                            {
                                if(win32State->InputRecordingIndex == 0)
                                {
                                    Win32BeginRecordingInput(win32State, 1);
                                }
                                else
                                {
                                    Win32EndRecordingInput(win32State);
                                    Win32BeginInputPlayback(win32State, 1);
                                }
                            }
                            else
                            {
                                Win32EndInputPlayback(win32State);
                            }
                            
                        }
                    }
                }           
                bool32 AltKeyWasDown = (Message.lParam & (1 << 29));
                if((VKCode == VK_F4) && AltKeyWasDown)
                {
                    GlobalRunning = false;
                }       
                if(VKCode == VK_RETURN && AltKeyWasDown && IsDown)
                {
                    ToggleFullScreen(Message.hwnd);
                }
            } break;                              
            default:
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);                                
            }                            
        }                                              
    }
}

internal void Win32AddWorkEntry(platform_work_queue *queue, platform_work_queue_func *func, void *data)
{
    uint32 nextEntryToWrite = (queue->NextEntryToWrite + 1) % ArrayCount(queue->Entries);
    Assert(nextEntryToWrite != queue->NextEntryToRead);
    
    platform_work_queue_entry *entry = queue->Entries + queue->NextEntryToWrite;
    entry->Func = func;
    entry->Data = data;
    
    ++queue->CompletionGoal;
    // NOTE(Joey): make sure compiler ensures all previous code finished its writes before continuing.
    _WriteBarrier(); 
    queue->NextEntryToWrite = nextEntryToWrite;    
    
    ReleaseSemaphore(queue->SemaphoreHandle, 1, 0);
}

internal bool32 Win32ProcessWorkQueueEntry(platform_work_queue *queue)
{
    bool32 shouldSleep = false;
    
    uint32 originalNextEntrytoRead = queue->NextEntryToRead;
    uint32 newNextEntryToRead = (originalNextEntrytoRead + 1) % ArrayCount(queue->Entries);
    if(originalNextEntrytoRead != queue->NextEntryToWrite)
    {
        // NOTE(Joey): compares queue->NextEntryToRead to originalNextEntrytoRead and if they are equal
        // queue->NextEntryToRead is exchanged with newNextEntryToRead; the function returns the initial
        // value of queue->NextEntryToRead; if this is the original value as we read before, we know only
        // one thread incremented the value. If a different thread also reads the NextEntryToRead at aangeroepen
        // similar time it will have been incremented, and the original value returned is not the same as 
        // originalNextEntryToRead and thus nothing happens.
        uint32 index = InterlockedCompareExchange((LONG volatile *)&queue->NextEntryToRead,
                                                  newNextEntryToRead,
                                                  originalNextEntrytoRead);
        if(index == originalNextEntrytoRead)
        {
            platform_work_queue_entry entry = queue->Entries[index];
            entry.Func(queue, entry.Data);
            InterlockedIncrement((LONG volatile *)&queue->CompletionCount);
        }
    }
    else
    {
        shouldSleep = true;
    }
    
    return shouldSleep;
}

internal void Win32CompleteAllWork(platform_work_queue *queue)
{
    while(queue->CompletionGoal != queue->CompletionCount)
        Win32ProcessWorkQueueEntry(queue);
    
    queue->CompletionGoal = 0;
    queue->CompletionCount = 0;
}

DWORD threadFunc(LPVOID lpParameter)
{
    platform_work_queue *queue = (platform_work_queue *)lpParameter;
    
    for(;;)
    {
        if(Win32ProcessWorkQueueEntry(queue))
        {
            WaitForSingleObjectEx(queue->SemaphoreHandle, INFINITE, FALSE);
        }
    }
    
    // return 0;
}

internal void Win32MakeWorkQueue(platform_work_queue *queue, uint32 threadCount)
{ 
    queue->CompletionGoal = 0;
    queue->CompletionCount = 0;
    
    queue->NextEntryToRead = 0;
    queue->NextEntryToWrite = 0;
    
    uint32 initialCount = 0;
    queue->SemaphoreHandle = CreateSemaphoreEx(0, initialCount, threadCount, 0, 0, SEMAPHORE_ALL_ACCESS);
    
    for(uint32 i = 0; i < threadCount; ++i)
    {
        DWORD threadID;
        HANDLE threadHandle = CreateThread(0, 0, threadFunc, queue, 0, &threadID);
        CloseHandle(threadHandle);
    }
}


void WorkTestEntry(platform_work_queue *queue, void *data)
{
    OutputDebugStringA((const char*)data);    
}

int CALLBACK WinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	LPSTR cmdLine,
	int cmdShow)
{
    win32_state win32State = {};
    
    Win32GetEXEFileName(&win32State);
    char sourceGameCodeDLLFullPath[MAX_PATH];
    Win32BuildEXEPathFileName(&win32State, "voidt.dll", sizeof(sourceGameCodeDLLFullPath), sourceGameCodeDLLFullPath);
    char tempGameCodeDLLFullPath[MAX_PATH];
    Win32BuildEXEPathFileName(&win32State, "voidt_temp.dll", sizeof(tempGameCodeDLLFullPath), tempGameCodeDLLFullPath);
    char lockGameCodeDLLFullPath[MAX_PATH];
    Win32BuildEXEPathFileName(&win32State, "lock.tmp", sizeof(lockGameCodeDLLFullPath), lockGameCodeDLLFullPath);
    
	// MessageBox(0, "Sup G", "Joey", MB_OK | MB_ICONINFORMATION);	
    Win32LoadXInput();
    
#if INTERNAL
    DEBUGGlobalShowCursor = true;
#endif
    
	WNDCLASS WindowClass = {};
    
    Win32ResizeDIBSection(&GlobalBackBuffer, 960, 540);
	
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
	WindowClass.hInstance = instance;
	WindowClass.lpszClassName = "JoeyWindowClass";
    WindowClass.hCursor = LoadCursor(0, IDC_CROSS);
		
	// LARGE_INTEGER perfCountFrequency;
    QueryPerformanceFrequency(&GlobalPerfCountFrequency);
    // sets the scheduler granularity to 1ms so that our
    // sleep() is more accurate
    UINT desiredSchedulerMS = 1;
    bool32 sleepIsGranular = timeBeginPeriod(desiredSchedulerMS) == TIMERR_NOERROR;
         
    
    // initialize thread work queue(s)
    platform_work_queue queueHighPriority = {};
    Win32MakeWorkQueue(&queueHighPriority, 6);
    platform_work_queue queueLowPriority = {};
    Win32MakeWorkQueue(&queueLowPriority, 2);
    
        
	if(RegisterClass(&WindowClass))
	{
        // NOTE(Joey): calculate client size of window and pass to window creation routine        
		HWND Window = CreateWindowEx(
			0, //WS_EX_TOPMOST|WS_EX_LAYERED,
			WindowClass.lpszClassName,
			"Joey game",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			960 + 13,
			540 + 35,
			0,
			0,
			instance,
			0
		);
		
		if(Window)
		{			    
            // query monitor refresh rate
            int monitorRefreshHz = 60;
            HDC dc = GetDC(Window);
            int win32RefreshRate = GetDeviceCaps(dc, VREFRESH);
            ReleaseDC(Window, dc);
            if(win32RefreshRate > 1)
                monitorRefreshHz = win32RefreshRate;
            real32 gameUpdateHz = (real32)(monitorRefreshHz / 2.0f);
            real32 targetSecondsPerFrame = 1.0f / (real32)gameUpdateHz;
    
    
    
            // graphics testbed variables
            int XOffset = 0;
            int YOffset = 0;
            
            // sound testbed variables           
            win32_sound_output soundOutput = {};
                
            soundOutput.samplesPerSecond    = 48000;
            soundOutput.runningSampleIndex  = 0;
            soundOutput.bytesPerSample      = sizeof(int16) * 2;
            soundOutput.secondaryBufferSize = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;
            // soundOutput.latencySampleCount  = 3 * (soundOutput.samplesPerSecond / gameUpdateHz);
            soundOutput.SafetyBytes         = (int)(((real32)soundOutput.samplesPerSecond*(real32)soundOutput.bytesPerSample / gameUpdateHz) / 2.0f);

            Win32InitDSound(Window, soundOutput.samplesPerSecond, soundOutput.secondaryBufferSize); 
            Win32ClearBuffer(&soundOutput);
            GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
           
            // NOTE(Joey): allocate memory for samples, add padding bits of 4 samples (SIMD)
            u32 maxSampleOverrun = 2*4*sizeof(u16);
            int16 *samples = (int16 *)VirtualAlloc(0, soundOutput.secondaryBufferSize + maxSampleOverrun, 
                                                   MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            
            #if INTERNAL
                LPVOID baseAddress       = (LPVOID)Gigabytes(2048);
            #else
                LPVOID baseAddress       = 0;
            #endif
            game_memory gameMemory = {};
            gameMemory.PermanentStorageSize = Megabytes(64);
            gameMemory.TransientStorageSize = Megabytes(64) /*Gigabytes(1)*/;
            win32State.TotalSize            = gameMemory.PermanentStorageSize + gameMemory.TransientStorageSize;
            win32State.GameMemoryBlock      = VirtualAlloc(baseAddress, (size_t)win32State.TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            // win32State.GameReplayMemoryBlock= VirtualAlloc(0, (size_t)win32State.TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            gameMemory.PermanentStorage     = win32State.GameMemoryBlock;
            gameMemory.TransientStorage     = (uint8 *)gameMemory.PermanentStorage + gameMemory.PermanentStorageSize;            
            gameMemory.DEBUGPlatformFreeFileMemory  = DEBUGPlatformFreeFileMemory;
            gameMemory.DEBUGPlatformReadEntireFile  = DEBUGPlatformReadEntireFile;
            gameMemory.DEBUGPlatformWriteEntireFile = DEBUGPlatformWriteEntireFile;
            gameMemory.PlatformWriteDebugOutput     = Win32WriteDebugOutput;
            
            gameMemory.WorkQueueHighPriority        = &queueHighPriority;
            gameMemory.WorkQueueLowPriority         = &queueLowPriority;
            gameMemory.PlatformAddWorkEntry         = Win32AddWorkEntry;
            gameMemory.PlatformCompleteAllWork      = Win32CompleteAllWork;
            
            for(int i = 0; i < 4; ++i)
            {
                win32_replay_buffer *replayBuffer = &win32State.ReplayBuffers[i];
                replayBuffer->MemoryBlock = VirtualAlloc(0, (size_t)win32State.TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                Assert(replayBuffer->MemoryBlock);
            }
            
            if(samples && gameMemory.PermanentStorage && gameMemory.TransientStorage)
            {
            
                game_input input[2] = {};
                game_input *newInput = &input[0];
                game_input *oldInput = &input[1];               
                
                bool32 soundIsValid = false;
                DWORD audioLatencyBytes = 0;
                real32 audioLatencySeconds = 0.0f;
                
                
                GlobalRunning = true;                
                LARGE_INTEGER lastCounter = Win32GetWallClock();
                LARGE_INTEGER flipWallClock = Win32GetWallClock();
                
                win32_game_code game = Win32LoadGameCode(sourceGameCodeDLLFullPath, tempGameCodeDLLFullPath, lockGameCodeDLLFullPath);
                

                uint64 lastCycleCount = __rdtsc();                       
                while(GlobalRunning)
                {              
                    newInput->dtPerFrame = targetSecondsPerFrame;
             
                    FILETIME newDLLWriteTime = Win32GetLastWriteTime(sourceGameCodeDLLFullPath);
                    if(CompareFileTime(&game.DLLLastWriteTime, &newDLLWriteTime) != 0)
                    {
                        Win32CompleteAllWork(&queueHighPriority);
                        Win32CompleteAllWork(&queueLowPriority);
                        Win32UnloadGameCode(&game);
                        game = Win32LoadGameCode(sourceGameCodeDLLFullPath, tempGameCodeDLLFullPath, lockGameCodeDLLFullPath);
                    }
            
                    //////////////////////////////////////////////////////////
                    //       INPUT
                    //////////////////////////////////////////////////////////         
                    game_controller_input *oldKeyboardController = GetController(oldInput, 0);
                    game_controller_input *newKeyboardController = GetController(newInput, 0);
                    *newKeyboardController = {};
                    newKeyboardController->IsConnected = true;
                    for(int buttonIndex = 0; buttonIndex < ArrayCount(newKeyboardController->Buttons); ++buttonIndex)
                    {
                        newKeyboardController->Buttons[buttonIndex].EndedDown = oldKeyboardController->Buttons[buttonIndex].EndedDown;
                    }
                    
                    Win32ProcessPendingMessages(&win32State, newKeyboardController);
                    
                    // poll mouse input
                    POINT mouseP;
                    GetCursorPos(&mouseP);
                    ScreenToClient(Window, &mouseP);
                    newInput->MouseX = mouseP.x;
                    newInput->MouseY = mouseP.y;
                    newInput->MouseZ = 0;
                    Win32ProcessKeyboardMessage(&newInput->MouseButtons[0], GetKeyState(VK_LBUTTON) & (1 << 15));
                    Win32ProcessKeyboardMessage(&newInput->MouseButtons[1], GetKeyState(VK_MBUTTON) & (1 << 15));
                    Win32ProcessKeyboardMessage(&newInput->MouseButtons[2], GetKeyState(VK_RBUTTON) & (1 << 15));
                    Win32ProcessKeyboardMessage(&newInput->MouseButtons[3], GetKeyState(VK_XBUTTON1) & (1 << 15));
                    Win32ProcessKeyboardMessage(&newInput->MouseButtons[4], GetKeyState(VK_XBUTTON2) & (1 << 15));
                    
                    // poll xbox 360 controller(s)
                    DWORD maxControllerCount = XUSER_MAX_COUNT;
                    if(maxControllerCount > (ArrayCount(newInput->Controllers) - 1))
                    {
                        maxControllerCount = ArrayCount(newInput->Controllers);
                    }
                    for(DWORD i = 0; i < maxControllerCount; ++i)
                    {
                        DWORD ourControllerIndex = i + 1;
                        game_controller_input *oldController = GetController(oldInput, ourControllerIndex);
                        game_controller_input *newController = GetController(newInput ,ourControllerIndex);
                        
                        XINPUT_STATE ControllerState;
                        if(XInputGetState(i, &ControllerState) == ERROR_SUCCESS)
                        {
                            // the controller is plugged in
                            XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;       
                            newController->IsConnected = true;                            
                            newController->IsAnalog = oldController->IsAnalog;                            
                            
                            // thumbsticks
                            real32 stickX = 0.0f;
                            real32 stickY = 0.0f;                            
                            // - deadzone processing and normalize to [-1, 1] range       
                            if(Pad->sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                                stickX = (real32)(Pad->sThumbLX + XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)  / (32768.0f - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                            else if(Pad->sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                                stickX = (real32)(Pad->sThumbLX - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)  / (32767.0f - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                            if(Pad->sThumbLY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
                                stickY = (real32)(Pad->sThumbLY + XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) / (32768.0f - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
                            else if(Pad->sThumbLY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
                                stickY = (real32)(Pad->sThumbLY - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) / (32767.0f - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);      
                            // - store stick values in controller state
                            newController->StickAverageX = stickX;
                            newController->StickAverageY = stickY;               
                            if(newController->StickAverageX != 0.0f || newController->StickAverageY != 0.0f)
                                newController->IsAnalog = true;
                            // - override stick values if d-pad button(s) are pressed
                            if(Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP)    { newController->StickAverageY =  1.0f; newController->IsAnalog = false; }
                            if(Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN)  { newController->StickAverageY = -1.0f; newController->IsAnalog = false; }
                            if(Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT)  { newController->StickAverageX = -1.0f; newController->IsAnalog = false; }
                            if(Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) { newController->StickAverageX =  1.0f; newController->IsAnalog = false; }                            
                            // - then process stick values as buttons
                            real32 threshold = 0.5f;
                            Win32ProcessXInputDigitalButton(newController->StickAverageX < -threshold, &oldController->MoveLeft, 1, &newController->MoveLeft);
                            Win32ProcessXInputDigitalButton(newController->StickAverageX >  threshold, &oldController->MoveRight, 1, &newController->MoveRight);
                            Win32ProcessXInputDigitalButton(newController->StickAverageY < -threshold, &oldController->MoveDown, 1, &newController->MoveDown);
                            Win32ProcessXInputDigitalButton(newController->StickAverageY >  threshold, &oldController->MoveUp, 1, &newController->MoveUp);
                                                        
                            // controller buttons
                            Win32ProcessXInputDigitalButton(Pad->wButtons, &oldController->LeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER, &newController->LeftShoulder);
                            Win32ProcessXInputDigitalButton(Pad->wButtons, &oldController->RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER, &newController->RightShoulder);
                            Win32ProcessXInputDigitalButton(Pad->wButtons, &oldController->ActionDown, XINPUT_GAMEPAD_A, &newController->ActionDown);
                            Win32ProcessXInputDigitalButton(Pad->wButtons, &oldController->ActionRight, XINPUT_GAMEPAD_B, &newController->ActionRight);
                            Win32ProcessXInputDigitalButton(Pad->wButtons, &oldController->ActionUp, XINPUT_GAMEPAD_Y, &newController->ActionUp);
                            Win32ProcessXInputDigitalButton(Pad->wButtons, &oldController->ActionLeft, XINPUT_GAMEPAD_Y, &newController->ActionLeft);           
                            Win32ProcessXInputDigitalButton(Pad->wButtons, &oldController->Start, XINPUT_GAMEPAD_START, &newController->Start);                               
                            Win32ProcessXInputDigitalButton(Pad->wButtons, &oldController->Back, XINPUT_GAMEPAD_BACK, &newController->Back);
                        }
                        else
                        {
                            // the controller is not available
                            newController->IsConnected = false;
                        }
                    }

                                      
                              
                
                    
                    //////////////////////////////////////////////////////////
                    //       UPDATE & RENDER 
                    //////////////////////////////////////////////////////////    
                    thread_context thread = {};
                    
                    game_offscreen_buffer buffer = {};
                    buffer.Memory = GlobalBackBuffer.Memory;
                    buffer.Width  = (uint16)GlobalBackBuffer.Width;
                    buffer.Height = (uint16)GlobalBackBuffer.Height;
                    buffer.Pitch  = GlobalBackBuffer.Pitch;                    
                    
                    // record/re-play user input for debug sessions
                    if(win32State.InputRecordingIndex)
                        Win32RecordInput(&win32State, newInput);
                    if(win32State.InputPlayingIndex)
                        Win32PlaybackInput(&win32State, newInput);
                    game.UpdateAndRender(&thread, &gameMemory, newInput, &buffer);
                    
                   
                    
                    //////////////////////////////////////////////////////////
                    //       AUDIO 
                    //////////////////////////////////////////////////////////              
                    LARGE_INTEGER audioWallClock = Win32GetWallClock();
                    real32 fromBeginToAudioSeconds = Win32GetSecondsElapsed(flipWallClock, audioWallClock);
                    
                    DWORD playCursor;
                    DWORD writeCursor; // safest distance from playCursor to write to
                    if(GlobalSecondaryBuffer->GetCurrentPosition(&playCursor, &writeCursor) == DS_OK)
                    {
                         /* NOTE(Joey:)
                    
                           This a nice comment syntax for large explenation of more
                           complicated logic. Start with a note and denote a three 
                           space left padding.
                       
                           This should eventually be a comment describing the logic
                           of the audio synchronization system.
                    
                        */
                        if(!soundIsValid)
                        {
                            soundOutput.runningSampleIndex = writeCursor / soundOutput.bytesPerSample;
                            soundIsValid = true;
                        }                                                
                        
                        DWORD byteToLock = (soundOutput.runningSampleIndex * soundOutput.bytesPerSample) % soundOutput.secondaryBufferSize;
                        
                        DWORD expectedSoundBytesPerFrame = (int)((real32)(soundOutput.samplesPerSecond * soundOutput.bytesPerSample) / gameUpdateHz);

                        real32 secondsLeftUntilFlip = targetSecondsPerFrame - fromBeginToAudioSeconds;
                        DWORD expectedBytesUntilFlip = (DWORD)((secondsLeftUntilFlip / targetSecondsPerFrame) * (real32)expectedSoundBytesPerFrame);                        
                        DWORD expectedFrameBoundaryByte = playCursor + expectedBytesUntilFlip;

                        
                        DWORD safeWriteCursor = writeCursor;
                        if(safeWriteCursor < playCursor)
                            safeWriteCursor += soundOutput.secondaryBufferSize;
                        Assert(safeWriteCursor >= playCursor);
                        safeWriteCursor += soundOutput.SafetyBytes;
                        
                        bool32 audioCardIsLowLatency = safeWriteCursor < expectedFrameBoundaryByte;

                        DWORD targetCursor = 0;       
                        if(audioCardIsLowLatency)
                            targetCursor = expectedFrameBoundaryByte + expectedSoundBytesPerFrame;
                        else
                            targetCursor = writeCursor + expectedSoundBytesPerFrame + soundOutput.SafetyBytes;
                        targetCursor = targetCursor % soundOutput.secondaryBufferSize;
                        
                        DWORD bytesToWrite = 0;
                        if(byteToLock > targetCursor)
                        {
                            bytesToWrite = (soundOutput.secondaryBufferSize - byteToLock); // from byteToLock to end
                            bytesToWrite += targetCursor;                                  // from 0 to position at PlayCursor
                        }
                        else
                        {
                            bytesToWrite = targetCursor - byteToLock;
                        }
                        
                        // let game write to audio buffer
                        game_sound_output_buffer soundBuffer = {};
                        soundBuffer.SamplesPerSecond = soundOutput.samplesPerSecond;
                        soundBuffer.SampleCount = bytesToWrite / soundOutput.bytesPerSample;
                        soundBuffer.Samples = samples;
                        game.GetSoundSamples(&thread, &gameMemory, &soundBuffer);
                        
                        
                        // sync sound buffer with audio hardware                        
                        // DWORD unwrappedWriteCursor = writeCursor;
                        // if(unwrappedWriteCursor < playCursor) // write cursor circled atound ringbuffer while playCursor yet hasn'table
                            // unwrappedWriteCursor += soundOutput.secondaryBufferSize;
                        // audioLatencyBytes = unwrappedWriteCursor - playCursor;
                        // audioLatencySeconds = (((real32)audioLatencyBytes / (real32)soundOutput.bytesPerSample) / (real32)soundOutput.samplesPerSecond);
                                        
                        Win32FillSoundBuffer(&soundOutput, byteToLock, bytesToWrite, &soundBuffer);                        
                    }
                    else
                    {
                        soundIsValid = false;
                    }
                                    
                                    
                                                       

                    
                    //////////////////////////////////////////////////////////
                    //       TIMING 
                    //////////////////////////////////////////////////////////                  
                    LARGE_INTEGER workCounter = Win32GetWallClock();
                    real32 workSecondsElapsed = Win32GetSecondsElapsed(lastCounter, workCounter);
                    // - time synchronization                                      
                    real32 secondsElapsedForFrame = workSecondsElapsed;
                    if(secondsElapsedForFrame < targetSecondsPerFrame)
                    {
                        #if 0
                        if(sleepIsGranular)
                        {
                            DWORD sleepMS = (DWORD)(1000.0f * (targetSecondsPerFrame - secondsElapsedForFrame) - 0.5f); // - 0.5 due to sleep not accurate enough on exact ms
                            if(sleepMS > 0)  Sleep(sleepMS);                        
                        }
                        while(secondsElapsedForFrame < targetSecondsPerFrame)
                        {                 
                            secondsElapsedForFrame = Win32GetSecondsElapsed(lastCounter, Win32GetWallClock());
                        }
                        #endif
                        DWORD sleepMS = (DWORD)(1000.0f * (targetSecondsPerFrame - secondsElapsedForFrame));
                        if(sleepMS > 0)                                
                            Sleep(sleepMS);
                        while(secondsElapsedForFrame < targetSecondsPerFrame)
                        {                 
                            secondsElapsedForFrame = Win32GetSecondsElapsed(lastCounter, Win32GetWallClock());
                        }
                    }
                    else
                    {   // Missed frame rate! 
                        // TODO(Joey): diagnostics - log this!                        
                    }                    
                                     
                    // get timer stats at end of frame
                    LARGE_INTEGER endCounter = Win32GetWallClock();
                    real32 msPerFrame        = 1000.0f * Win32GetSecondsElapsed(lastCounter, endCounter);
                    lastCounter = endCounter;     
                 
                    // display frame to monitor (w/ blit)
                    win32_window_dimension Dimension = Win32GetWindowDimension(Window);     
                    HDC DeviceContext = GetDC(Window);
                    Win32DisplayBufferInWindow(DeviceContext, &GlobalBackBuffer, Dimension.Width, Dimension.Height);           
                    ReleaseDC(Window, DeviceContext);
                  
                    flipWallClock = Win32GetWallClock();
                  
                    // swap input
                    game_input *temp = newInput;
                    newInput = oldInput;
                    oldInput = temp;                                     
                    // timer - cycles
                    uint64 endCycleCount = __rdtsc();
                    uint64 cyclesElapsed  = endCycleCount - lastCycleCount;
                    lastCycleCount = endCycleCount;                    
                    // timer - print debug timing info
                    real64 fps           = (real64)GlobalPerfCountFrequency.QuadPart / (real64)endCounter.QuadPart;
                    real64 mcpf          = (real64)(cyclesElapsed / (1000.0f * 1000.0f)); // mega-cycles per frame                    
                    char charBuffer[256];
                    _snprintf_s(charBuffer, sizeof(charBuffer), "%.02fms/f / %.02ff/s  -  %.02fmc/f\n", msPerFrame, fps, mcpf); 
                    OutputDebugStringA(charBuffer);
  
                }
            }
            else
            {
                // diagnostics: memory allocation failed
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
