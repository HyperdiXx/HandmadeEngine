
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

internal void 
drawGradient(game_offscreen_buffer *Buffer, int XOffset, int YOffset)
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
drawPlayer(game_offscreen_buffer* Buffer, int PlayerX, int PlayerY, uint32 Color)
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
drawRectangle(game_offscreen_buffer* Buffer, real32 MinXIn, real32 MinYIn, real32 MaxXIn, real32 MaxYIn,
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
        int32 PlayerTileX = TruncateValue((TestX - World->UpperLeftX) / World->TileSideInPixels);
        int32 PlayerTileY = TruncateValue((TestY - World->UpperLeftY) / World->TileSideInPixels);

        if ((PlayerTileX >= 0) && (PlayerTileX < World->CountX) && (PlayerTileY >= 0) && (PlayerTileY < World->CountY))
        {
            //uint32 TileMapValue = GetTileValue(World, TileMap, PlayerTileX, PlayerTileY);
           // Empty = (TileMapValue == 0);
        }
    }

    return (Empty);
}

inline void
makePosCanonical(world *World, int32 TileCount, int32 *tileMap, int32 *tile, real32 *realVal)
{ 
   int32 offset = FloorReal32toInt32(*realVal / World->TileSideInPixels);
   *tile += offset;
   *realVal -= offset * World->TileSideInPixels;
    
    Assert(*realVal >= 0);
    Assert(*realVal < World->TileSideInPixels);

    if (*tile < 0)
    {
        *tile = TileCount + *tile;
        ++*tileMap;
    }

    if (*tile >= TileCount)
    {
        *tile = *tile - TileCount;
        ++*tileMap;

    }   
}

inline world_location
GetCanonicalLocation(world *World, world_location worldpos)
{
    world_location resPos = worldpos;

    makePosCanonical(World, World->TileMapCountX, &resPos.TileMapX, &resPos.TileX, &resPos.X);
    makePosCanonical(World, World->TileMapCountY, &resPos.TileMapY, &resPos.TileY, &resPos.Y);

    return (resPos);
}

internal bool32
IsWorldPointEmpty(world *World, world_location Testpos)
{
    bool32 Empty = false;

    tile_map *TileMap = GetTileMap(World, Testpos.TileMapX, Testpos.TileMapY);
    Empty = IsTileMapPointEmpty(World, TileMap, (real32)Testpos.TileX, (real32)Testpos.TileY);

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

    World.TileSideInPixels = 77;

    real32 PlayerW = 0.2f * World.TileSideInPixels;
    real32 PlayerH = 0.3f * (real32)World.TileSideInPixels;

    World.TileMaps = (tile_map*)TileMaps;

    game_state *GameState = (game_state *)Memory->PermanentStorage;

    if (!Memory->IsInitialized)
    {
        GameState->playerPosition.TileMapX = 0;
        GameState->playerPosition.TileMapY = 5;
        GameState->playerPosition.TileX = 0;
        GameState->playerPosition.TileY = 0;
        GameState->playerPosition.X = 120.0f;
        GameState->playerPosition.Y = 105.0f;

        GameState->vel = 256.0f;
        Memory->IsInitialized = true;
    }

    tile_map *TileMap = GetTileMap(&World, GameState->playerPosition.TileMapX, GameState->playerPosition.TileMapY);
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
            
            playerAddX *= 64.0f;
            playerAddY *= 64.0f;

            world_location PlayerPos = GameState->playerPosition;
            PlayerPos.X += GameState->vel * Input->TimeElapsed * (Controllers->StickAverageX);
            PlayerPos.Y += GameState->vel * Input->TimeElapsed * (Controllers->StickAverageY);
            
            GameState->playerPosition = GetCanonicalLocation(&World, GameState->playerPosition);

            world_location PlayerLeft = PlayerPos;
            PlayerLeft.X -= 0.5f * PlayerW;
            PlayerLeft = GetCanonicalLocation(&World, PlayerLeft);

            world_location PlayerRight = PlayerPos;
            PlayerRight.X += 0.5f * PlayerW;

            PlayerRight = GetCanonicalLocation(&World, PlayerRight);

            if (IsWorldPointEmpty(&World, GameState->playerPosition) && IsWorldPointEmpty(&World, PlayerLeft)
                && IsWorldPointEmpty(&World, PlayerRight))
            {
                GameState->playerPosition = PlayerPos;
            }
            else
            {

            }
        }
 
        /*if(Controllers->ActionDown.IsEnd)
        {
            real32 posX = (real32)GameState->PlayerX + 10.0f;
            real32 posY = (real32)GameState->PlayerY + 10.0f;
            RenderRectangle(Buffer, posX, posY, (real32)GameState->PlayerX + 20.0f, (real32)GameState->PlayerY + 20.0f, 1.0f, 1.0f, 1.0f);
            posX += (int)(Input->TimeElapsed * 64.0f);
        }*/
    }

    drawRectangle(Buffer, 0.0f, 0.0f, (real32)Buffer->Width, (real32)Buffer->Height, 1.0f, 1.0f, 1.0f);

    /*for (int Row = 0;
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
                Gray = 1.0f;

            real32 MinX = World.UpperLeftX + ((real32)Column) * World.TileSideInPixels;
            real32 MinY = World.UpperLeftY + ((real32)Row) * World.TileSideInPixels;
            real32 MaxX = MinX + World.TileSideInPixels;
            real32 MaxY = MinY + World.TileSideInPixels;
            drawRectangle(Buffer, MinX, MinY, MaxX, MaxY, Gray, Gray, Gray);
        }
    }*/
    
    //Color scheme
    real32 PlayerR = 1.0f;
    real32 PlayerG = 1.0f;
    real32 PlayerB = 0.0f;

    real32 PlayerLeft = World.UpperLeftX + World.TileSideInPixels * GameState->playerPosition.TileX + GameState->playerPosition.X - 0.5f * PlayerW;
    real32 PlayerTop = World.UpperLeftY + World.TileSideInPixels * GameState->playerPosition.TileY + GameState->playerPosition.Y - PlayerH;

    drawRectangle(Buffer, PlayerLeft, PlayerTop, PlayerLeft + PlayerW, PlayerTop + PlayerH, PlayerR, PlayerG, PlayerB);

    if (Input->Mouse[0].IsEnd)
    {
        drawPlayer(Buffer, 20, 20, 0xFFFF00FF);
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