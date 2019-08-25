

#if !defined(HANDMADE_H)

#include "handmade_platform.h"


#define internal static
#define local static
#define global static


#define Pi32 3.14159265359f

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


inline uint32
SafeTruncate(uint64 Val)
{
    Assert(Val <= 0xFFFFFFFF);
    uint32 Result = (uint32)Val;
    return (Result);
}

inline game_controller_input *GetController(game_input *Input, int ControllerIndex)
{
	Assert(ControllerIndex < ArrayCount(Input->Controlls));
	game_controller_input *Result = &Input->Controlls[ControllerIndex];
	return(Result);
}

struct world_location
{
    int32 TileMapX;
    int32 TileMapY;

    int32 TileX;
    int32 TileY;


    //NOTE(vlad): This is tile-relative x and y
    real32 X;
    real32 Y;
};

struct raw_position
{
    int32 TileMapX;
    int32 TileMapY;

    //NOTE(vlad): Tilem-map relative x and y
    real32 X;
    real32 Y;
};


struct tile_map
{
    uint32 *Map;
};

struct world
{
    real32 TileSideInMetr;
    int32 TileSideInPixels;

    int32 CountX;
    int32 CountY;

    int32 UpperLeftX;
    int32 UpperLeftY;

    int32 TileMapCountX;
    int32 TileMapCountY;

    tile_map *TileMaps;
};

struct game_state
{
    world_location playerPosition;
    /*int32 PlayerTileMapX;
    int32 PlayerTileMapY;
    int32 PlayerX, PlayerY;
    real32 tJmp;
    */
    real32 vel;
    int GreenOffset;
    int BlueOffset;
    int ToneHz;
    real32 tSine;
};




#define HANDMADE_H
#endif
