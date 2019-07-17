
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
GameOutputSound(game_state* GameState, game_sound_output_buffer *SoundBuffer, int ToneHz)
{
#if 0 
	int16 ToneVolume = 3000;
	int WavePeriod = SoundBuffer->SamplesPerSecond / ToneHz;

	int16 *SampleOut = SoundBuffer->Samples;

	for (int SampleIndex = 0;
		SampleIndex < SoundBuffer->SampleCount;
		++SampleIndex)
	{	
		real32 SineValue = sinf(GameState->tSine);
		int16 SampleValue = (int16)(SineValue * ToneVolume);
		*SampleOut++ = SampleValue;
		*SampleOut++ = SampleValue;

		GameState->tSine += 2.0f * Pi32 * 1.0f / (real32)WavePeriod;
        if (GameState->tSine > 2.0f * Pi32)
        {
            GameState->tSine -= 2.0f * Pi32;
        }
	}
#endif
}

internal void RenderGradient(game_offscreen_buffer *Buffer, int XOffset, int YOffset)
{
	int Width = Buffer->Width;
	int Height = Buffer->Height;

    uint8 BlueDarkConst = (uint8)64;

	uint8 *Row = (uint8 *)Buffer->BitmapMemory;
	for (int Y = 0; Y < Height; ++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for (int X = 0; X < Width; ++X)
		{
			//uint8 Blue = (uint8)(X + XOffset);
			uint8 Green = (uint8)(Y + YOffset);
            

			*Pixel++ = (BlueDarkConst);
		}

		Row += Buffer->Pitch;
	}
}

internal void
RenderPlayer(game_offscreen_buffer* Buffer, int PlayerX, int PlayerY, uint32 Color)
{
    uint8 *EndofBuffer = (uint8 *)Buffer->BitmapMemory + 
        Buffer->Pitch *
        Buffer->Height;
    int Top = PlayerY;
    int Bottom = PlayerY + 10;

    for (int x = PlayerX;
        x < PlayerX + 10;
        ++x)
    {
        uint8 *PlayerImage = ((uint8*)Buffer->BitmapMemory + 
                            x * Buffer->BytesPerPixel +
                            Top * Buffer->Pitch);

            for (int y = Top;
                y < Bottom;
                ++y)
            {
                if (PlayerImage >= Buffer->BitmapMemory &&
                    (PlayerImage < EndofBuffer))
                {
                    *(uint32 *)PlayerImage = Color;
                }
                
                PlayerImage += Buffer->Pitch;
            }
    }

}

#include <math.h>

inline int32
RoundValue(real32 Val)
{
    int32 Res = (int32)(Val + 0.5f);

    return (Res);
}

#include "math.h"
inline int32
FloorValue(real32 val)
{
    int32 res = (int32)floorf(val);
    return (res);
}

inline int32
TruncateValue(real32 Val)
{
    int32 Res = (int32)(Val);

    return (Res);
}

inline int32
FloorReal32toInt32(real32 Real)
{
    int32 Res = (int32)floorf(Real);
    return (Res);
}

internal void
RenderRectangle(game_offscreen_buffer* Buffer, real32 MinXIn, real32 MinYIn, real32 MaxXIn, real32 MaxYIn,
    real32 Red, real32 Green, real32 Blue)
{
    int32 MinY = RoundValue(MinYIn);
    int32 MaxY = RoundValue(MaxYIn);
    int32 MinX = RoundValue(MinXIn);
    int32 MaxX = RoundValue(MaxXIn);

    if (MinY < 0)
    {
        MinY = 0;
    }
    if (MinX < 0)
    {
        MinX = 0;
    }
    if (MaxX > Buffer->Width)
    {
        MaxY = Buffer->Width;
    }
    if (MaxY > Buffer->Height)
    {
        MaxX = Buffer->Height;
    }

    uint32 Color = (uint32)((RoundValue(Red * 255.0f) << 16) |
                    (RoundValue(Green * 255.0f) << 8) |
                    (RoundValue(Blue *  255.0f)));

    
    uint8 *Row = ((uint8 *)Buffer->BitmapMemory + MinX * Buffer->BytesPerPixel + MinY * Buffer->Pitch);

    // : Cache friendly

    for (int y = MinY;
        y < MaxY;
        ++y)
    {
        uint32 *Pixel = (uint32 *)Row;
        for (int x = MinX;
            x < MaxX;
            ++x)
        {
            *Pixel++ = Color;
        }
        Row += Buffer->Pitch;
    }
    
}

inline tile_map*
GetTileMap(world *World, int32 TileX, int32 TileY)
{
    tile_map *TileMap = 0;
    if ((TileX >= 0) && (TileX < World->CountX) && (TileY >= 0) && (TileY < World->CountY))
    {
        TileMap = &World->TileMaps[TileY * World->CountX + TileX];
    }
    return (TileMap);
}

inline uint32
GetTileValue(world *World, tile_map *TileMap, int32 TileX, int32 TileY)
{
    Assert(TileMap);
    Assert((TileX >= 0) && (TileX < World->CountX) && (TileY >= 0) && (TileY < World->CountY));

    uint32 TileMapVal = TileMap->Map[TileY * World->CountX + TileX];
    return(TileMapVal);
}

internal bool32
IsTileMapPointEmpty(world *World, tile_map *TileMap, real32 TestX, real32 TestY)
{
    bool32 Empty = false;

    if (TileMap)
    {
        int32 PlayerTileX = TruncateValue((TestX - World->UpperLeftX) / World->TileWidth);
        int32 PlayerTileY = TruncateValue((TestY - World->UpperLeftY) / World->TileHeight);

        if ((PlayerTileX >= 0) && (PlayerTileX < World->CountX) && (PlayerTileY >= 0) && (PlayerTileY < World->CountY))
        {
            uint32 TileMapValue = GetTileValue(World, TileMap, PlayerTileX, PlayerTileY);
            Empty = (TileMapValue == 0);
        }

    }

    return (Empty);
}



inline world_location
GetCanonicalLocation(world *World, raw_position worldpos)
{
    world_location resPos;

    resPos.TileMapX = worldpos.TileMapX;
    resPos.TileMapY = worldpos.TileMapY;

    real32 X = worldpos.X - World->UpperLeftX;
    real32 Y = worldpos.Y - World->UpperLeftY;

    resPos.TileX = TruncateValue(X / World->TileWidth);
    resPos.TileY = TruncateValue(Y / World->TileHeight);

    resPos.X = worldpos.X - resPos.TileX * World->TileWidth;
    resPos.Y = worldpos.Y - resPos.TileY * World->TileHeight;

    //Assert(resPos.X >= 0);
    //Assert(resPos.Y >= 0);
    //Assert(resPos.X < World->TileWidth);
    //Assert(resPos.X < World->TileHeight);


    if (resPos.TileX < 0)
    {
        resPos.TileX = World->CountX + resPos.TileX;
        --resPos.TileMapX;
    }

    if (resPos.TileY < 0)
    {
        resPos.TileY = World->CountY + resPos.TileY;
        --resPos.TileMapY;
    }

    if (resPos.TileX >= World->CountX)
    {
        resPos.TileX = resPos.TileX - World->CountX;
        ++resPos.TileMapX;

    }

    if (resPos.TileY >= World->CountY)
    {
        resPos.TileY = resPos.TileX - World->CountX;
        ++resPos.TileMapY;
    }

    return (resPos);
}

internal bool32
IsWorldPointEmpty(world *World, raw_position Testpos)
{
    bool32 Empty = false;

    world_location pos = GetCanonicalLocation(World, Testpos);

    tile_map *TileMap = GetTileMap(World, pos.TileMapX, pos.TileMapY);
    Empty = IsTileMapPointEmpty(World, TileMap, (real32)pos.TileX, (real32)pos.TileY);

    return (Empty);
}




extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert((&Input->Controlls[0].Terminator - &Input->Controlls[0].Buttons[0]) ==
        (ArrayCount(Input->Controlls[0].Buttons)));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);

#define TILEMAPCOUNTX 16
#define TILEMAPCOUNTY 9



    uint32 Map1[TILEMAPCOUNTY][TILEMAPCOUNTX] =
    {
        {1, 0, 0, 0,    1, 0, 0, 0,    0, 0, 1, 0,    0, 0, 0, 1},
        {1, 1, 1, 0,    1, 0, 0, 0,    0, 0, 1, 1,    1, 1, 1, 1},
        {0, 0, 1, 0,    1, 0, 0, 0,    0, 0, 1, 1,    1, 1, 0, 0},
        {0, 0, 1, 1,    1, 1, 1, 0,    0, 0, 1, 0,    1, 0, 0, 0},
        {0, 0, 1, 0,    1, 0, 1, 0,    0, 1, 1, 0,    0, 1, 0, 0},
        {0, 1, 0, 0,    1, 0, 1, 0,    0, 1, 1, 0,    0, 0, 1, 0},
        {1, 0, 0, 0,    1, 1, 1, 0,    0, 1, 1, 0,    0, 0, 1, 1},
        {0, 1, 0, 0,    0, 0, 1, 1,    0, 1, 1, 0,    0, 0, 1, 0},
        {0, 0, 1, 0,    0, 0, 0, 1,    1, 1, 1, 1,    1, 1, 1, 0},
    };

    uint32 Map2[TILEMAPCOUNTY][TILEMAPCOUNTX] =
    {
        {1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 0, 1, 1,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 0, 1, 1,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 0, 1, 0,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 1, 1, 0,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 1, 1, 0,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 1, 1, 0,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 1,    0, 1, 1, 0,    0, 0, 0, 1},
        {1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1},
    };

    uint32 Map3[TILEMAPCOUNTY][TILEMAPCOUNTX] =
    {
        {1, 0, 0, 0,    1, 0, 0, 0,    0, 0, 1, 0,    0, 0, 0, 1},
        {1, 1, 1, 0,    1, 0, 0, 0,    0, 0, 1, 1,    1, 1, 1, 1},
        {0, 0, 1, 0,    1, 0, 0, 0,    0, 0, 1, 1,    1, 1, 0, 0},
        {0, 0, 1, 1,    1, 1, 1, 0,    0, 0, 1, 0,    1, 0, 0, 0},
        {0, 0, 1, 0,    1, 0, 1, 0,    0, 1, 1, 0,    0, 1, 0, 0},
        {0, 1, 0, 0,    1, 0, 1, 0,    0, 1, 1, 0,    0, 0, 1, 0},
        {1, 0, 0, 0,    1, 1, 1, 0,    0, 1, 1, 0,    0, 0, 1, 1},
        {0, 1, 0, 0,    0, 0, 1, 1,    0, 1, 1, 0,    0, 0, 1, 0},
        {0, 0, 1, 0,    0, 0, 0, 1,    1, 1, 1, 1,    1, 1, 1, 0},
    };

    uint32 Map4[TILEMAPCOUNTY][TILEMAPCOUNTX] =
    {
        {1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 0, 1, 1,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 0, 1, 1,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 0, 1, 0,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 1, 1, 0,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 1, 1, 0,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 0,    0, 1, 1, 0,    0, 0, 0, 1},
        {1, 0, 0, 0,    0, 0, 1, 1,    0, 1, 1, 0,    0, 0, 0, 1},
        {1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1},
    };

    tile_map TileMaps[2][2];
 
   
    world World = {};

    TileMaps[0][0].Map = (uint32 *)Map1;
    TileMaps[0][1].Map = (uint32 *)Map2;
    TileMaps[1][0].Map = (uint32 *)Map3;
    TileMaps[1][1].Map = (uint32 *)Map4;

    World.TileMapCountX = 2;
    World.TileMapCountY = 2;
    World.CountX = TILEMAPCOUNTX;
    World.CountY = TILEMAPCOUNTY;
    World.UpperLeftX = -20;
    World.UpperLeftY = -10;

    World.TileWidth = 77;
    World.TileHeight = 77;


    real32 PlayerW = 0.2f * World.TileWidth;
    real32 PlayerH = 0.3f * (real32)World.TileHeight;


    World.TileMaps = (tile_map*)TileMaps;

    game_state *GameState = (game_state *)Memory->PermanentStorage;
    

    if (!Memory->IsInitialized)
    {
        GameState->PlayerX = 50;
        GameState->PlayerY = 50;
        GameState->vel = 256.0f;
        Memory->IsInitialized = true;
    }


    tile_map *TileMap = GetTileMap(&World, GameState->PlayerTileMapX, GameState->PlayerTileMapY);
    Assert(TileMap);

    for (int ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controlls);
        ++ControllerIndex)
    {
        game_controller_input *Controllers = GetController(Input, ControllerIndex);
        if (Controllers->IsAnalog)
        {
            //GameState->ToneHz = 512 + (int)(128.0f * (Controllers->StickAverageY));
            //GameState->BlueOffset += (int)(4.0f * (Controllers->StickAverageX));
        }
        else
        {

            real32 playerAddX = 0.0f;
            real32 playerAddY = 0.0f;

            if (Controllers->MoveUp.IsEnd)
            {
                playerAddY = -1.0f;
            }

            if (Controllers->MoveDown.IsEnd)
            {
                playerAddY = 1.0f;
            }

            if (Controllers->MoveLeft.IsEnd)
            {
                playerAddX = -1.0f;
            }

            if (Controllers->MoveRight.IsEnd)
            {
                playerAddX = 1.0f;
            }

        }

        GameState->PlayerX += (int32)(GameState->vel * Input->TimeElapsed * (Controllers->StickAverageX));
        GameState->PlayerY -= (int32)(GameState->vel * Input->TimeElapsed * (Controllers->StickAverageY));

        bool32 IsMoved = false;

        for (int i = 0;
            (i < 4) && (!IsMoved);
            ++i)
        {

            real32 NewPlayerX = GameState->PlayerX + GameState->vel * Input->TimeElapsed * (Controllers->StickAverageX);
            real32 NewPlayerY = GameState->PlayerY + GameState->vel * Input->TimeElapsed * (Controllers->StickAverageY);

            raw_position PlayerPos = {
            GameState->PlayerTileMapX, GameState->PlayerTileMapY, NewPlayerX, NewPlayerY };

            raw_position PlayerLeft = PlayerPos;
            PlayerLeft.X -= 0.5f * PlayerW;

            raw_position PlayerRight = PlayerPos;
            PlayerRight.X += 0.5f * PlayerW;

            if (IsWorldPointEmpty(&World, PlayerPos) && IsWorldPointEmpty(&World, PlayerLeft)
                && IsWorldPointEmpty(&World, PlayerRight))
            {

                world_location pos = GetCanonicalLocation(&World, PlayerPos);

                GameState->PlayerTileMapX = pos.TileMapX;
                GameState->PlayerTileMapY = pos.TileMapY;

                GameState->PlayerX = World.UpperLeftX + World.TileWidth * pos.TileX + (int32)pos.X;
                GameState->PlayerY = World.UpperLeftY + World.TileHeight * pos.TileY + (int32)pos.Y;
                IsMoved = true;
            }
            else
            {

            }

        }              
 
        if(Controllers->ActionDown.IsEnd)
        {
            real32 posX = (real32)GameState->PlayerX + 10.0f;
            real32 posY = (real32)GameState->PlayerY + 10.0f;
            RenderRectangle(Buffer, posX, posY, (real32)GameState->PlayerX + 20.0f, (real32)GameState->PlayerY + 20.0f, 1.0f, 1.0f, 1.0f);
            posX += (int)(Input->TimeElapsed * 64.0f);
        }
    }

   
    for (int Row = 0;
        Row < 9;
        ++Row)
    {
        for (int Column = 0;
            Column < 16;
            ++Column)
        {
            uint32 ID = GetTileValue(&World, TileMap, Column, Row);
            real32 Gray = 0.5f;
            if (ID == 1)
            {
                Gray = 1.0f;
            }
            real32 MinX = World.UpperLeftX + ((real32)Column) * World.TileWidth;
            real32 MinY = World.UpperLeftY + ((real32)Row) * World.TileHeight;
            real32 MaxX = MinX + World.TileWidth;
            real32 MaxY = MinY + World.TileHeight;
            RenderRectangle(Buffer, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);

        }
    }

    real32 PlayerLeft = GameState->PlayerX - 0.5f * PlayerW;
    real32 PlayerTop = GameState->PlayerY - PlayerH;

    //RenderPlayer(Buffer, GameState->PlayerX, GameState->PlayerY, 0xFFFFFFFF);

    RenderRectangle(Buffer, PlayerLeft, PlayerTop, PlayerLeft + PlayerW, PlayerTop + PlayerH, 0.0f, 0.0f, 0.6f);


    if (Input->Mouse[0].IsEnd)
    {
        RenderPlayer(Buffer, 20, 20, 0xFFFF0000);
    }
}


extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
  game_state *GameState = (game_state *)Memory->PermanentStorage;
  GameOutputSound(GameState, SoundBuffer, GameState->ToneHz);

}

#if HANDMADE_WIN32
#include <windows.h>
BOOL WINAPI DllMain(
    _In_ HINSTANCE hInstDll,
    _In_ DWORD dw,
    _In_ LPVOID lpRes)
{
    
    return (TRUE);
}
#endif