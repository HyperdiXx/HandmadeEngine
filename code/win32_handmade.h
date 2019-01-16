
#if !defined(WIN32_HANDMADE_H)


struct win32_offscreen_buffer
{
	BITMAPINFO BitmapInfo;;
	void* BitmapMemory;
	int BitmapWidth;
	int BitmapHeight;
	int Pitch;
	int BytesPerPixel;
};

struct win32_window_dimension
{
	int Width;
	int Height;
};

struct win32_sound_output
{
	int SamplesPerSecond;
	int SquareWaveCounter;
	int WavePeriod;
	int BytesPerSample;
	int SecondaryBufferSize;
	uint32 RunningSampleIndex;
	real32 tSine;
	int LatencySampleCount;
    DWORD SafetyBytes;
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
