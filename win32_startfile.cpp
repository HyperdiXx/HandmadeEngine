
#include <math.h>
#include <stdint.h>


#define internal static
#define local static
#define global static

#define Pi32 3.14159265359f

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef float real32;
typedef double real64;


#include <windows.h>
#include <xinput.h>
#include <stdio.h>
#include <stdint.h>
#include <dsound.h>

#include "handmade.h"
#include "handmade.cpp"

#include "win32_handmade.h"

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pDoubleState)
typedef X_INPUT_GET_STATE(x_input_get_state);
typedef X_INPUT_SET_STATE(x_input_set_state);

X_INPUT_GET_STATE(XInputGetStateStub)
{
	return(ERROR_DEVICE_NOT_CONNECTED);
}

X_INPUT_SET_STATE(XInputSetStateStub)
{
	return(ERROR_DEVICE_NOT_CONNECTED);
}

global x_input_get_state *XInputGetState_ = XInputGetStateStub;
global x_input_set_state *XInputSetState_ = XInputSetStateStub;

#define XInputGetState XInputGetState_
#define XInputSetState XInputSetState_

global bool32 Running;
global bool32 GlobalPause;
global win32_offscreen_buffer GlobalBackBuffer;
global LPDIRECTSOUNDBUFFER GlobalSecondBuffer;
global int64 PerfCounter;


#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnlOuter);
typedef DIRECT_SOUND_CREATE(direct_sound_create);




internal debug_read_file_res DEBUGPlatformReadEntireFile(char *Filename)
{
	debug_read_file_res Result = {};

	HANDLE FileHandle = CreateFileA(
									Filename,
									GENERIC_READ, FILE_SHARE_READ,
									0, OPEN_EXISTING, 0, 0);
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER FileSize;
		if (GetFileSizeEx(FileHandle, &FileSize))
		{
			uint32 FileSize32 = SafeTruncate(FileSize.QuadPart);
			Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (Result.Contents)
			{
				DWORD BytesRead;
				if (ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0) && (FileSize32 == BytesRead))
				{
					Result.ContentsSize = FileSize32;
				}
				else
				{
					DEBUGPlatformFreeFileMemory(Result.Contents);
					Result.Contents = 0;
				}
			}
			else
			{

			}
		}
		else
		{

		}
			
		CloseHandle(FileHandle);

	}

	return(Result);
}
internal void DEBUGPlatformFreeFileMemory(void *Memory)
{
	if (Memory)
	{
		VirtualFree(Memory, 0, MEM_RELEASE);
	}
}
internal bool32 DEBUGPlatformWriteEntireFile(char* Filename, uint32 MemorySize, void *Memory)
{
	bool32 Result = false;

	HANDLE FileHandle = CreateFileA(
		Filename,
		GENERIC_WRITE, 0,
		0, CREATE_ALWAYS, 0, 0);
	if (FileHandle != INVALID_HANDLE_VALUE)
	{
		DWORD BytesWriten;
		if (WriteFile(FileHandle, Memory, MemorySize, &BytesWriten, 0))
		{
			Result = (BytesWriten == MemorySize);
		}
		else
		{

		}

		CloseHandle(FileHandle);
	}
	else
	{

	}
	return(Result);
}


internal void
Win32LoadXInput(void)
{
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if (!XInputLibrary)
	{
		XInputLibrary = LoadLibraryA("xinput9_1_0.dll");
	}
	if(!XInputLibrary)
	{
		XInputLibrary = LoadLibraryA("xinput1_3.dll");
	}
	if (XInputLibrary)
	{
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		if (!XInputGetState) { XInputGetState = XInputGetStateStub; }
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
		if (!XInputSetState) { XInputSetState = XInputSetStateStub; }
	}
}



internal 
win32_window_dimension Win32GetWindowDimension(HWND Window)
{
	win32_window_dimension Result;
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;

	return (Result);
}


internal void
Win32InitDSound(HWND Window, int32 SamplesPerSecond, int32 BufferSize)
{
	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

	if (DSoundLibrary)
	{
		direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary,
			"DirectSoundCreate");

		LPDIRECTSOUND DirectSound;

		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
		{
			WAVEFORMATEX WaveFormat = {};
			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormat.nChannels = 2;
			WaveFormat.nSamplesPerSec = SamplesPerSecond;
			WaveFormat.wBitsPerSample = 16;
			WaveFormat.nBlockAlign = WaveFormat.nChannels * WaveFormat.wBitsPerSample / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
			WaveFormat.cbSize = 0;


			if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
			{
				DSBUFFERDESC BuffDesc1 = {};
				BuffDesc1.dwSize = sizeof(BuffDesc1);
				BuffDesc1.dwFlags = DSBCAPS_PRIMARYBUFFER;

				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BuffDesc1, &PrimaryBuffer, 0)))
				{

					HRESULT Error = PrimaryBuffer->SetFormat(&WaveFormat);
					if (SUCCEEDED(Error))
					{
						OutputDebugStringA("Primary buffer set\n");
					}
					else
					{


					}
				}
				else
				{


				}
			}
			else
			{

			}

			DSBUFFERDESC BuffDesc = {};
			BuffDesc.dwSize = sizeof(BuffDesc);
			BuffDesc.dwFlags = 0;
			BuffDesc.dwBufferBytes = BufferSize;
			BuffDesc.lpwfxFormat = &WaveFormat;
			HRESULT Error = DirectSound->CreateSoundBuffer(&BuffDesc, &GlobalSecondBuffer, 0);
			if (SUCCEEDED(Error))
			{
				OutputDebugStringA("Secondary buffer set\n");
			}
		}
		else
		{
			//Errors
		}
	}
	else
	{

	}
}


internal void
Win32ClearBufferSound(win32_sound_output *SoundOutput)
{
	VOID *Region1;
	DWORD Region1Size;
	VOID *Region2;
	DWORD Region2Size;

	if (SUCCEEDED(GlobalSecondBuffer->Lock(
		0,
		SoundOutput->SecondaryBufferSize,
		&Region1,
		&Region1Size,
		&Region2,
		&Region2Size,
		0)))
	{
		uint8 *DestSample = (uint8 *)Region1;

		for (DWORD ByteIndex = 0;
			ByteIndex < Region1Size;
			++ByteIndex)
		{
			*DestSample++ = 0;
		}

		DestSample = (uint8 *)Region2;

		for (DWORD ByteIndex = 0;
			ByteIndex < Region2Size;
			++ByteIndex)
		{
			*DestSample++ = 0;
		}

		GlobalSecondBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
	} 
}

internal void 
Win32FillBufferSound(win32_sound_output *SoundOutput, game_sound_output_buffer *SourceBuffer, 
	DWORD ByteToLock, DWORD BytesToWrite)
{
		VOID *Region1;
		DWORD RegionSize1;
		VOID *Region2;
		DWORD RegionSize2;

		if (SUCCEEDED(GlobalSecondBuffer->Lock(
			ByteToLock,
			BytesToWrite,
			&Region1,
			&RegionSize1,
			&Region2,
			&RegionSize2,
			0)))
		{

			DWORD Region1SampleCount = RegionSize1 / SoundOutput->BytesPerSample;

			int16 *DestSample = (int16 *)Region1;
			int16 *SampleSource = SourceBuffer->Samples;

			for (DWORD SampleIndex = 0;
				SampleIndex < Region1SampleCount;
				++SampleIndex)
			{
				*DestSample++ = *SampleSource++;
				*DestSample++ = *SampleSource++;

				++SoundOutput->RunningSampleIndex;
			}

			DWORD Region2SampleCount = RegionSize2 / SoundOutput->BytesPerSample;
			DestSample = (int16 *)Region2;

			for (DWORD SampleIndex = 0;
				SampleIndex < Region2SampleCount;
				++SampleIndex)
			{
				*DestSample++ = *SampleSource++;
				*DestSample++ = *SampleSource++;

				++SoundOutput->RunningSampleIndex;
			}

			GlobalSecondBuffer->Unlock(Region1, RegionSize1, Region2, RegionSize2);
		}
}

internal real32
Win32ProcessStick(SHORT Value, SHORT DeadZone)
{
	real32 Result = 0;
	if (Value < -DeadZone)
	{
		Result = (real32)(Value + DeadZone) / (32768.0f - DeadZone);
	}
	else if (Value > DeadZone)
	{
		Result = (real32)(Value + DeadZone)/ (32767.0f - DeadZone);
	}
	return (Result);
}

internal void
Win32ProcessKeyboard(game_button_state *OldC, bool32 Is)
{
	Assert(OldC->IsEnd != Is);
	OldC->IsEnd = Is;
	++OldC->HalfCount;
}

internal void
Win32ProcessButton(DWORD XInputButtonState, game_button_state *OldState, DWORD ButtonBit, 
					game_button_state *NewState)
{
	NewState->IsEnd = ((XInputButtonState & ButtonBit) == ButtonBit);
	NewState->HalfCount = (OldState->IsEnd != NewState->IsEnd) ? 1 : 0;
}


internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
	if (Buffer->BitmapMemory)
	{
		VirtualFree(Buffer->BitmapMemory, 0, MEM_RELEASE);
	}

	Buffer->BitmapWidth = Width;
	Buffer->BitmapHeight = Height;

	int BytesPerPixel = 4;
	
	Buffer->BytesPerPixel = BytesPerPixel;

	Buffer->BitmapInfo.bmiHeader.biSize = sizeof(Buffer->BitmapInfo.bmiHeader);
	Buffer->BitmapInfo.bmiHeader.biWidth = Buffer->BitmapWidth;
	Buffer->BitmapInfo.bmiHeader.biHeight = -Buffer->BitmapHeight;
	Buffer->BitmapInfo.bmiHeader.biPlanes = 1;
	Buffer->BitmapInfo.bmiHeader.biBitCount = 32;
	Buffer->BitmapInfo.bmiHeader.biCompression = BI_RGB;


	int BitmapMemorySize = (Buffer->BitmapWidth * Buffer->BitmapHeight) * Buffer->BytesPerPixel;

	Buffer->BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	Buffer->Pitch = Width * Buffer->BytesPerPixel;
}

internal void
Win32UpdateWindow(win32_offscreen_buffer *Buffer, HDC DeviceContext, int W, int H)
{
	StretchDIBits(
			DeviceContext, 
			/*X, Y, Width, Height,
			X, Y, Width, Height,*/
			0, 0, W, H,
			0, 0, Buffer->BitmapWidth, Buffer->BitmapHeight,
			Buffer->BitmapMemory, &Buffer->BitmapInfo,
			DIB_RGB_COLORS, SRCCOPY);
}

internal void
Win32MessageLoop(game_controller_input *KeyboardController)
{
	MSG Message;
	while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		switch (Message.message)
		{
		case WM_QUIT:
		{
			Running = false;
		}break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{

			uint32 VKCode = (uint32)Message.wParam;
			bool WasDown = ((Message.lParam & (1 << 30)) != 0);
			bool IsDown = ((Message.lParam & (1 << 31)) == 0);
			if (WasDown != IsDown)
			{
				if (VKCode == 'W')
				{
					Win32ProcessKeyboard(&KeyboardController->MoveUp, IsDown);
				}
				else if (VKCode == 'A')
				{
					Win32ProcessKeyboard(&KeyboardController->MoveLeft, IsDown);
				}
				else if (VKCode == 'D')
				{
					Win32ProcessKeyboard(&KeyboardController->MoveRight, IsDown);

				}
				else if (VKCode == 'S')
				{
					Win32ProcessKeyboard(&KeyboardController->MoveDown, IsDown);

				}
				else if (VKCode == 'Q')
				{
					Win32ProcessKeyboard(&KeyboardController->LeftShoulder, IsDown);

				}
				else if (VKCode == 'E')
				{
					Win32ProcessKeyboard(&KeyboardController->RightShoulder, IsDown);

				}
				else if (VKCode == VK_UP)
				{
					Win32ProcessKeyboard(&KeyboardController->ActionUp, IsDown);
				}
				else if (VKCode == VK_DOWN)
				{
					Win32ProcessKeyboard(&KeyboardController->ActionDown, IsDown);

				}
				else if (VKCode == VK_LEFT)
				{
					Win32ProcessKeyboard(&KeyboardController->ActionLeft, IsDown);

				}
				else if (VKCode == VK_RIGHT)
				{
					Win32ProcessKeyboard(&KeyboardController->ActionRight, IsDown);

				}
				else if (VKCode == VK_ESCAPE)
				{
					Running = false;
					Win32ProcessKeyboard(&KeyboardController->Start, IsDown);
				}
				else if (VKCode == VK_SPACE)
				{
					Win32ProcessKeyboard(&KeyboardController->Back, IsDown);
				}
#if HANDMADE_INTERNAL
				else if (VKCode == 'P')
				{
					if (IsDown)
					{
						GlobalPause = !GlobalPause;
					}
				}
#endif

			}

			bool32 AltKeyDown = ((Message.lParam & (1 << 29)) != 0);
			if ((VKCode == VK_F4) && AltKeyDown)
			{
				Running = false;
			}
		}break;

		default:
		{
			TranslateMessage(&Message);
			DispatchMessageA(&Message);
		}break;
		}
		
	}

}


LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
	UINT Message,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT Result;

	switch (Message)
	{
		case WM_SIZE:
		{
			
		}break;

		case WM_DESTROY:
		{
			Running = false;
			OutputDebugStringA("WM_DESTROY\n");
		}break;

		case WM_CLOSE:
		{
			//PostQuitMessage(0);
			Running = false;
		}break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		}break;

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			Assert(!"Keyboard pressed");
		}break;

		case WM_PAINT:
		{
			
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);

			win32_window_dimension Dimension = Win32GetWindowDimension(Window);

			Win32UpdateWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);
			EndPaint(Window, &Paint);
		}break;

		default:
		{
			Result = DefWindowProc(Window, Message, wParam, lParam);
		}break;
	}

	return (Result);
}

inline LARGE_INTEGER
Win32GetWallClock(void)
{
	LARGE_INTEGER EndCounter;

	QueryPerformanceCounter(&EndCounter);

	return(EndCounter);
}

internal void 
Win32DebugDrawVertical(win32_offscreen_buffer *GloBuffer, int X, int Top, int Bottom, uint32 Color)
{
	if (Top <= 0)
	{
		Top = 0;
	}

	if (Bottom > GloBuffer->BitmapHeight)
	{
		Bottom = GloBuffer->BitmapHeight;
	}

	if ((X >= 0) && (X < GloBuffer->BitmapWidth))
	{
		uint8 *Pixel = ((uint8 *)GloBuffer->BitmapMemory +
			X * GloBuffer->BytesPerPixel +
			Top * GloBuffer->Pitch);
		for (int Y = Top;
			Y < Bottom;
			++Y)
		{
			*(uint32 *)Pixel = Color;
			Pixel += GloBuffer->Pitch;
		}
	}
	
}

inline void
Win32DrawSoundBufferMarker(win32_offscreen_buffer *Buffer, win32_sound_output *Sound,
					real32 C, int PadX, int Top, int Bottom, DWORD Value, uint32 Color)
{
	//Assert(Value < Sound->SecondaryBufferSize);
	real32 XReal32 = (C * (real32)Value);
	int X = PadX + (int)XReal32;
	if ((X >= 0) && (X < Buffer->BitmapWidth))
	{
		Win32DebugDrawVertical(Buffer, X, Top, Bottom, Color);
	}
}
	

internal void
Win32DebugSyncDisplay(win32_offscreen_buffer *GloBuffer, int MarkersCount,
	win32_debug_time_marker *Markers, int CurrentMarkerIndex,
	win32_sound_output *Sound, real32 SecondsPerFrame)
{
	int PadY = 16;
	int PadX = 16;

	int LineHeight = 64;
	
	real32 C = (real32)(GloBuffer->BitmapWidth - 2 * PadX) / (real32)Sound->SecondaryBufferSize;

	for (int PlayCursorIndex = 0;
		PlayCursorIndex < MarkersCount;
		++PlayCursorIndex)
	{
		win32_debug_time_marker *ThisMarker = &Markers[PlayCursorIndex];
		Assert(ThisMarker->OutputPlayCursor < Sound->SecondaryBufferSize);
		Assert(ThisMarker->OutputWriteCursor < Sound->SecondaryBufferSize);
		Assert(ThisMarker->OutputLocation < Sound->SecondaryBufferSize);
		Assert(ThisMarker->OutputByteCount < Sound->SecondaryBufferSize);
		Assert(ThisMarker->ExpectedFlipPlayCursor < Sound->SecondaryBufferSize);
		Assert(ThisMarker->FlipPlayCursor < Sound->SecondaryBufferSize);
		Assert(ThisMarker->FlipWriteCursor < Sound->SecondaryBufferSize);
		
		DWORD PlayColor = 0xFFFFFFFF;
		DWORD WriteColor = 0xFFFF0000;
		DWORD ExpectedFlipColor = 0xFFFFFF00;

		

		int Top = PadY;
		int Bottom = PadY + LineHeight;



		if (PlayCursorIndex == CurrentMarkerIndex)
		{
			Top += LineHeight + PadY;
			Bottom += LineHeight + PadY;

			int FirstTop = Top;

			Win32DrawSoundBufferMarker(GloBuffer, Sound, C, PadX, Top, Bottom, ThisMarker->OutputPlayCursor, PlayColor);
			Win32DrawSoundBufferMarker(GloBuffer, Sound, C, PadX, Top, Bottom, ThisMarker->OutputWriteCursor, WriteColor);
			
			Top += LineHeight + PadY;
			Bottom += LineHeight + PadY;

			Win32DrawSoundBufferMarker(GloBuffer, Sound, C, PadX, Top, Bottom, ThisMarker->OutputLocation, PlayColor);
			Win32DrawSoundBufferMarker(GloBuffer, Sound, C, PadX, Top, Bottom, ThisMarker->OutputLocation + ThisMarker->OutputByteCount, WriteColor);

			Top += LineHeight + PadY;
			Bottom += LineHeight + PadY;

			Win32DrawSoundBufferMarker(GloBuffer, Sound, C, PadX, FirstTop, Bottom, ThisMarker->OutputWriteCursor, ExpectedFlipColor);
		}
		
		Win32DrawSoundBufferMarker(GloBuffer, Sound, C, PadX, Top, Bottom, ThisMarker->FlipPlayCursor, PlayColor);
		Win32DrawSoundBufferMarker(GloBuffer, Sound, C, PadX, Top, Bottom, ThisMarker->FlipWriteCursor, WriteColor);
	}
}



int CALLBACK WinMain(HINSTANCE hInstance,
	HINSTANCE hPrev,
	LPSTR lpCmdLine,
	int ncmdShow)
{
	LARGE_INTEGER pr;
	QueryPerformanceFrequency(&pr);

	PerfCounter = pr.QuadPart;

	UINT DesiredNumberDS = 1;
	bool32 SleepIs = timeBeginPeriod(DesiredNumberDS) == TIMERR_NOERROR;

	Win32LoadXInput();

	WNDCLASS WindowClass = {};

	Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = hInstance;
	WindowClass.lpszClassName = "EngineClass";
	
	#define MonitorRefreshRate 60
	#define GameUpdateHz MonitorRefreshRate / 2

	real32 TargetSecondsElapsedPerFrame = 1.0f / (real32)MonitorRefreshRate;


	if (RegisterClass(&WindowClass))
	{
		HWND Window =
			CreateWindowEx(
				0,
				WindowClass.lpszClassName,
				"Hero",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				0, //parentwindow
				0,
				hInstance,
				0);
		
		if (Window)
		{
			HDC DeviceContext = GetDC(Window);

			win32_sound_output SoundOutput = {};
			SoundOutput.SamplesPerSecond = 48000;
			SoundOutput.WavePeriod = SoundOutput.SamplesPerSecond / 256;
			SoundOutput.BytesPerSample = sizeof(int16) * 2;
			SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample;
			SoundOutput.LarencySampleCount = 3 * (SoundOutput.SamplesPerSecond / GameUpdateHz);
			SoundOutput.SafetyBytes = (SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample / GameUpdateHz) / 3;
			
			Win32InitDSound(Window, SoundOutput.SamplesPerSecond, SoundOutput.SecondaryBufferSize);
			Win32ClearBufferSound(&SoundOutput);
			GlobalSecondBuffer->Play(0, 0, DSBPLAY_LOOPING);

			Running = true;

			int16 *Samples = (int16 *)VirtualAlloc(0, SoundOutput.SecondaryBufferSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

#if HANDMADE_INTERNAL
			LPVOID BaseAddress = (LPVOID)Terabytes((uint64)1);
#else
			LPVOID BaseAddress = 0;
#endif

			game_memory GameMemory = {};

			GameMemory.PermanentStorageSize = Megabytes(64);
			GameMemory.TransientStorageSize = Gigabytes((uint64)4);

			uint64 TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;

			GameMemory.PermanentStorage = VirtualAlloc(0, (size_t)TotalSize,
				MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			GameMemory.TransientStorage = ((uint8 *)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize);
				
			if (Samples && GameMemory.PermanentStorage && GameMemory.TransientStorage)
			{


				game_input Input[2] = {};
				game_input *NewInput = &Input[0];
				game_input *OldInput = &Input[1];

				LARGE_INTEGER LastCounter = Win32GetWallClock();
				LARGE_INTEGER FlipWallClock = Win32GetWallClock();

				DWORD AudioLatencyBytes = 0;
				real32 AudioLatencySounds = 0;

				int DebugTimeMarkerIndex = 0;
				win32_debug_time_marker DebugTimeMarkers[GameUpdateHz] = { 0 };


				uint64 LastCycleCounter = __rdtsc();

				
				while (Running)
				{

					game_controller_input *OldKeyboard = GetController(OldInput, 0);
					game_controller_input *NewKeyboard = GetController(NewInput, 0);
					*NewKeyboard = {};
					NewKeyboard->IsConnected = true;

					for (int ButtonIndex = 0;
						ButtonIndex < ArrayCount(NewKeyboard->Buttons);
						++ButtonIndex)
					{
						NewKeyboard->Buttons[ButtonIndex].IsEnd = OldKeyboard->Buttons[ButtonIndex].IsEnd;
					}

					Win32MessageLoop(NewKeyboard);

					DWORD MaxUserCount = XUSER_MAX_COUNT;

					if (MaxUserCount > (ArrayCount(NewInput->Controlls) - 1))
					{
						MaxUserCount = (ArrayCount(NewInput->Controlls) - 1);
					}

					for (DWORD ControllerIndex = 0;
						ControllerIndex < MaxUserCount;
						++ControllerIndex)
					{
						DWORD OurControllerIndex = ControllerIndex + 1;
						game_controller_input *OldController = GetController(OldInput, 0);
						game_controller_input *NewController = GetController(NewInput, 0);

						XINPUT_STATE ControllerState;
						if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
						{
							NewKeyboard->IsConnected = true;

							XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

							NewController->IsAnalog = true;
							NewController->StickAverageX = Win32ProcessStick(Pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

							NewController->StickAverageY = Win32ProcessStick(Pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

							if ((NewController->StickAverageX != 0.0f) || (NewController->StickAverageY != 0.0f))
							{
								NewController->IsAnalog = true;
							}

							if (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP)
							{
								NewController->StickAverageY = 1.0f;
								NewController->IsAnalog = false;
							}
							if (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
							{
								NewController->StickAverageY = -1.0f;
								NewController->IsAnalog = false;
							}
							if (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
							{
								NewController->StickAverageX = -1.0f;
								NewController->IsAnalog = false;
							}
							if (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
							{
								NewController->StickAverageX = 1.0f;
								NewController->IsAnalog = false;
							}


							real32 The = 0.5f;

							Win32ProcessButton(NewController->StickAverageX < -The ? 1 : 0,
								&OldController->MoveLeft, 1,
								&NewController->MoveLeft);
							Win32ProcessButton(NewController->StickAverageX > -The ? 1 : 0,
								&OldController->MoveLeft, 1,
								&NewController->MoveLeft);
							Win32ProcessButton(NewController->StickAverageY < -The ? 1 : 0,
								&OldController->MoveLeft, 1,
								&NewController->MoveLeft);
							Win32ProcessButton(NewController->StickAverageY > -The ? 1 : 0,
								&OldController->MoveLeft, 1,
								&NewController->MoveLeft);


							Win32ProcessButton(Pad->wButtons, &OldController->ActionDown,
								XINPUT_GAMEPAD_A, &NewController->ActionDown);
							Win32ProcessButton(Pad->wButtons, &OldController->ActionRight,
								XINPUT_GAMEPAD_B, &NewController->ActionRight);
							Win32ProcessButton(Pad->wButtons, &OldController->ActionLeft,
								XINPUT_GAMEPAD_X, &NewController->ActionLeft);
							Win32ProcessButton(Pad->wButtons, &OldController->ActionUp,
								XINPUT_GAMEPAD_Y, &NewController->ActionUp);

							Win32ProcessButton(Pad->wButtons, &OldController->LeftShoulder,
								XINPUT_GAMEPAD_LEFT_SHOULDER, &NewController->LeftShoulder);

							Win32ProcessButton(Pad->wButtons, &OldController->RightShoulder,
								XINPUT_GAMEPAD_RIGHT_SHOULDER, &NewController->RightShoulder);

							Win32ProcessButton(Pad->wButtons, &OldController->Start,
								XINPUT_GAMEPAD_START, &NewController->Start);

							Win32ProcessButton(Pad->wButtons, &OldController->Back,
								XINPUT_GAMEPAD_BACK, &NewController->Back);
						}
						else
						{
							NewKeyboard->IsConnected = false;
						}
					}

				
					bool32 IsPlaying = false;


					game_offscreen_buffer Buffer = {};
					Buffer.BitmapMemory = GlobalBackBuffer.BitmapMemory;
					Buffer.Width = GlobalBackBuffer.BitmapWidth;
					Buffer.Height = GlobalBackBuffer.BitmapHeight;
					Buffer.Pitch = GlobalBackBuffer.Pitch;

					GameUpdateAndRenderer(&GameMemory, NewInput, &Buffer);

					LARGE_INTEGER AudioCounter = Win32GetWallClock();
					real32 FromBeginToAudioSeconds = (1000.0f * (real32)FlipWallClock.QuadPart - AudioCounter.QuadPart);
					
					DWORD PlayCursor;
					DWORD WriteCursor;
					if ((GlobalSecondBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)) == DS_OK)
					{

						if (!IsPlaying)
						{
							SoundOutput.RunningSampleIndex = WriteCursor / SoundOutput.BytesPerSample;
							IsPlaying = true;
						}

						DWORD BytesToLock = ((SoundOutput.RunningSampleIndex * SoundOutput.BytesPerSample) % SoundOutput.SecondaryBufferSize);

						DWORD BytesToWrite = 0;
						DWORD TargetCursor = 0;

						DWORD ExpectedSoundBytesPerFrame = (SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample) / GameUpdateHz;
						DWORD ExpectedFrameBoundaryByte = PlayCursor +
						  ExpectedSoundBytesPerFrame;

						real32 SecondsLeftUntilFlip = (TargetSecondsElapsedPerFrame - FromBeginToAudioSeconds);

						DWORD ExpectedBytesUntilFlip = (DWORD)(((TargetSecondsElapsedPerFrame - FromBeginToAudioSeconds) / ExpectedSoundBytesPerFrame) * (real32)ExpectedSoundBytesPerFrame);

						DWORD SafeWriteCursor = WriteCursor;
						if(SafeWriteCursor < PlayCursor)
						{
						  SafeWriteCursor += SoundOutput.SecondaryBufferSize;
						}
						Assert(SafeWriteCursor >= PlayCursor);
						SafeWriteCursor += SoundOutput.SafetyBytes;
						bool32 AudioCardIsLowLatency = (SafeWriteCursor < ExpectedFrameBoundaryByte);
					        
						if(AudioCardIsLowLatency)
						{
						     TargetCursor = ((ExpectedFrameBoundaryByte +
								   ExpectedSoundBytesPerFrame));
						}
						else
						{						  
						     TargetCursor = ((WriteCursor + ExpectedSoundBytesPerFrame + 
								      SoundOutput.SafetyBytes) %
						       SoundOutput.SecondaryBufferSize);
						}

						TargetCursor = (TargetCursor % SoundOutput.SecondaryBufferSize);
						
						if (BytesToLock > TargetCursor)
						{
							BytesToWrite = (SoundOutput.SecondaryBufferSize - BytesToLock);
							BytesToWrite += TargetCursor;
						}
						else
						{
							BytesToWrite = TargetCursor - BytesToLock;
						}

						game_sound_output_buffer SoundBuffer = {};

						SoundBuffer.SamplesPerSecond = SoundOutput.SamplesPerSecond;
						SoundBuffer.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
						SoundBuffer.Samples = Samples;

						GetSoundSamples(&GameMemory, &SoundBuffer);

#if HANDMADE_INTERNAL
						
						win32_debug_time_marker *Marker = &DebugTimeMarkers[DebugTimeMarkerIndex];
						Marker->OutputPlayCursor = PlayCursor;
						Marker->OutputWriteCursor = WriteCursor;
						Marker->OutputLocation = BytesToLock;
						Marker->OutputByteCount = BytesToWrite;
						//Marker->ExpectedFlipPlayCursor = ExpectedFlipColor;

						DWORD UnwrappedWriteCursor = WriteCursor;
						if (UnwrappedWriteCursor < PlayCursor)
						{
							UnwrappedWriteCursor += SoundOutput.SecondaryBufferSize;
						}
						AudioLatencyBytes = UnwrappedWriteCursor - PlayCursor; 
						AudioLatencySounds = (real32)((AudioLatencyBytes / SoundOutput.BytesPerSample) / SoundOutput.SamplesPerSecond);

#endif
						Win32FillBufferSound(&SoundOutput, &SoundBuffer, BytesToLock, BytesToWrite);
					}
					else
					{
						IsPlaying = false;
					}


					LARGE_INTEGER WorkCounter = Win32GetWallClock();

					int64 TimeElapsed = (WorkCounter.QuadPart - LastCounter.QuadPart);

					real32 SecondsElapsedForWork = ((real32)TimeElapsed / (real32)PerfCounter);

					real32 SecondsElapsedForFrame = SecondsElapsedForWork;
					if (SecondsElapsedForFrame < TargetSecondsElapsedPerFrame)
					{
						while (SecondsElapsedForFrame < TargetSecondsElapsedPerFrame)
						{
							if (SleepIs)
							{
								DWORD SleepMS = (DWORD)(1000.0f * (TargetSecondsElapsedPerFrame - SecondsElapsedForFrame));
								if (SleepMS > 0)
								{
									Sleep(SleepMS);
								}
							}
							LARGE_INTEGER CheckCounter = Win32GetWallClock();

							SecondsElapsedForFrame = ((real32)(CheckCounter.QuadPart - LastCounter.QuadPart) / (real32)PerfCounter);
						}

					}
					else
					{


					}

					LARGE_INTEGER EndCounter = Win32GetWallClock();
					real32 MSPerFrame = (((1000.0f * (real32)TimeElapsed) / (real32)PerfCounter));
					LastCounter = EndCounter;

					win32_window_dimension Dimension = Win32GetWindowDimension(Window);
#if HANDMADE_INTERNAL
					Win32DebugSyncDisplay(&GlobalBackBuffer, ArrayCount(DebugTimeMarkers), DebugTimeMarkers, DebugTimeMarkerIndex - 1, &SoundOutput, TargetSecondsElapsedPerFrame);
#endif
					Win32UpdateWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);

					FlipWallClock = Win32GetWallClock();
					
#if HANDMADE_INTERNAL
					{
					    DWORD PlayCursor2;
						DWORD WriteCursor2;

						
						if(GlobalSecondBuffer->GetCurrentPosition(&PlayCursor2, &WriteCursor2) == DS_OK)
						{
						    Assert(DebugTimeMarkerIndex < ArrayCount(DebugTimeMarkers));
						    win32_debug_time_marker *Marker = &DebugTimeMarkers[DebugTimeMarkerIndex++];

						   
						    Marker->FlipPlayCursor = PlayCursor2;
						    Marker->FlipWriteCursor = WriteCursor2;
						}
					}
#endif



#if 0
					
					real64 FPS = (real64)PerfCountFreq / (real64)TimeElapsed;
					real64 MCPF = ((real64)CyclesElapsed / (1000.0f * 1000.0f));

					char Buffer[256];
					sprintf(Buffer, "Mill/Frame: %fms / %fFPS  %fmc/f\n", MSPerFrame, FPS, MCPF);
					OutputDebugStringA(Buffer);
#endif
					game_input *T = NewInput;
					NewInput = OldInput;
					OldInput = T;

					

					uint64 EndCycleCounter = __rdtsc();
					uint64 CyclesElapsed = EndCycleCounter - LastCycleCounter;
					LastCycleCounter = EndCycleCounter;
#if HANDMADE_INTERNAL
					++DebugTimeMarkerIndex;
					if (DebugTimeMarkerIndex >= ArrayCount(DebugTimeMarkers))
					{
						DebugTimeMarkerIndex = 0;
					}

#endif
				}
			}
			else
			{

			}
		}
		else
		{

		}
	}
	else
	{

	}
	return (0);
}
