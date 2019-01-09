
#include "handmade.h"



internal game_state*
GameStartUp(void)
{
	game_state *GameState = new game_state;
	if (GameState)
	{
		GameState->GreenOffset = 0;
		GameState->BlueOffset = 0;
		GameState->ToneHz = 256;
	}
	return(GameState);
}


internal game_state*
GameShutdown(game_state *GameState)
{
	delete GameState;
}

internal void
GameOutputSound(game_sound_output_buffer *SoundBuffer, int ToneHz)
{
	local real32 tSine;
	int16 ToneVolume = 3000;
	int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz;

	int16 *SampleOut = SoundBuffer->Samples;

	for (int SampleIndex = 0;
		SampleIndex < SoundBuffer->SampleCount;
		++SampleIndex)
	{	
		real32 SineValue = sinf(tSine);
		int16 SampleValue = (int16)(SineValue * ToneVolume);
		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;

		tSine += 2.0f * Pi32 * 1.0f / (real32)WavePeriod;
	}
}

internal void RenderGradient(game_offscreen_buffer *Buffer, int XOffset, int YOffset)
{
	int Width = Buffer->Width;
	int Height = Buffer->Height;

	uint8 *Row = (uint8 *)Buffer->BitmapMemory;
	for (int Y = 0; Y < Height; ++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for (int X = 0; X < Width; ++X)
		{
			uint8 Blue = (uint8)(X + XOffset);
			uint8 Green = (uint8)(Y + YOffset);

			*Pixel++ = ((Green << 8) | Blue);
		}

		Row += Buffer->Pitch;
	}
}

internal void 
GameUpdateAndRenderer(game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer)
{
	Assert((&Input->Controlls[0].Terminator - &Input->Controlls[0].Buttons[0]) ==
		(ArrayCount(Input->Controlls[0].Buttons)));
	Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

	game_state *GameState = (game_state *)Memory->PermanentStorage;

	if (!Memory->IsInitialized)
	{
		char *Filename = __FILE__;

		debug_read_file_res File = DEBUGPlatformReadEntireFile(Filename);

		if (File.Contents)
		{
			DEBUGPlatformWriteEntireFile("V:/handmade/data/file.out", File.ContentsSize, File.Contents);
			DEBUGPlatformFreeFileMemory(File.Contents);
		}


		GameState->ToneHz = 256;

		Memory->IsInitialized = true;
	}

	for (int ControllerIndex = 0;
		ControllerIndex < ArrayCount(Input->Controlls);
		++ControllerIndex)
	{
		game_controller_input *Controllers = GetController(Input, ControllerIndex);
		if (Controllers->IsAnalog)
		{
			GameState->ToneHz = 256 - (int)(128.0f * (Controllers->StickAverageY));
			GameState->BlueOffset += (int)(4.0f * (Controllers->StickAverageX));
		}
		else
		{
			if (Controllers->MoveLeft.IsEnd)
			{
				GameState->BlueOffset -= (int)1;
			}
			if (Controllers->MoveRight.IsEnd)
			{
				GameState->BlueOffset += 1;
			}

		}
		if (Controllers->ActionDown.IsEnd)
		{
			GameState->GreenOffset += 1;
		}
	}

	RenderGradient(Buffer, GameState->BlueOffset, GameState->GreenOffset);
}

internal void
GetSoundSamples(game_memory *Memory, game_sound_output_buffer *SoundBuffer)
{
  game_state *GameState = (game_state *)Memory->PermanentStorage;
   GameOutputSound(SoundBuffer, GameState->ToneHz);

}
