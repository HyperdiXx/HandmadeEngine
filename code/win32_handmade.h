
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


struct win32_game_code
{
    HMODULE GameCodeDLL;
    FILETIME DLLLastWriteFile;
    game_update_and_render* UpdateAndRender;
    game_get_sound_samples* GameGetSoundSamples;

    bool32 IsValid;
};

#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH

struct win32_state
{
    game_memory Gamememory;
    uint64 TotalSize;
    void* GameMemoryBlock;

    HANDLE RecordingHandle;
    int InputRecordingIndex;

    HANDLE InputPlaybackHandle;
    int InputPlayBackIndex;

    char EXEFilename[WIN32_STATE_FILE_NAME_COUNT];
    char *OnePastLastEXEFileNameSlash;
};

#define WIN32_HANDMADE_H
#endif
