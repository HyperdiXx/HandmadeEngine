

#if !defined(HANDMADE_H)


 /* HANDMADE_INTERNAL:
  if 0 => Build for public usage
  if 1 => Build for debug

  HANDMADE_SLOW:
  0 - Not slow code;
  1 - Slow code can be
*/

#if HANDMADE_SLOW
#define Assert(Express) if(!(Express)) {*(int *) 0 = 0;}
#else
#define Assert(Express)
#endif

#define Kilobytes(Value) ((Value) * 1024)
#define Megabytes(Value) (Kilobytes(Value) * 1024)
#define Gigabytes(Value) (Megabytes(Value) * 1024)
#define Terabytes(Value) (Gigabytes(Value) * 1024)
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0])) 

#if HANDMADE_INTERNAL
struct debug_read_file_res
{
  uint32 ContentsSize;
  void *Contents;
};
internal debug_read_file_res DEBUGPlatformReadEntireFile(char *Filename);
internal void DEBUGPlatformFreeFileMemory(void *Memory);
internal bool32 DEBUGPlatformWriteEntireFile(char* Filename, uint32 MemorySize, void *Memory);
#endif


inline uint32
SafeTruncate(uint64 Val)
{
	Assert(Val <= 0xFFFFFFFF);
	uint32 Result = (uint32)Val;
	return (Result);
}

struct game_offscreen_buffer
{
    void *BitmapMemory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct game_sound_output_buffer
{
    int SampleCount;
    int SamplesPerSecond;
    int16 *Samples;
};

struct game_button_state
{
	int HalfCount;
	bool32 IsEnd;
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

			game_button_state Start;
			game_button_state Back;

			game_button_state Terminator;
		};
		
	};
	
};

struct game_input
{
	game_controller_input Controlls[5];
};

inline game_controller_input *GetController(game_input *Input, int ControllerIndex)
{
	Assert(ControllerIndex < ArrayCount(Input->Controlls));
	game_controller_input *Result = &Input->Controlls[ControllerIndex];
	return(Result);
}

struct game_memory
{
	bool32 IsInitialized;
	uint64 PermanentStorageSize;
	void *PermanentStorage;

	uint64 TransientStorageSize;
	void *TransientStorage;
};

struct game_state
{
	int GreenOffset;
	int BlueOffset;
	int ToneHz;
};


internal void GameUpdateAndRenderer(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer);

internal void GetSoundSamples(game_memory *Memory, game_sound_output_buffer *SoundBuffer);

#define HANDMADE_H
#endif
