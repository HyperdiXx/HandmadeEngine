@echo off

set CommonCompilerFlags=-MT -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4018 -wd4201 -wd4100 -wd4505 -wd4701 -wd4189 -DOPENGL_RENDER=0 -DSOFTWARE_BASED=0 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7 
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

REM 32-bit build
REM cl %CommonCompilerFlags% ..\handmade\code\win32_startfile.cpp /link -subsystem: windows,5.1 %CommonLinkerFlags%

REM 64-bit build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% ..\handmade\code\handmade.cpp -Fmhandmade.map -LD /link -incremental:no -opt:ref -PDB:handmade_%RANDOM%.pdb /DLL -EXPORT:GameGetSoundSamples -EXPORT:GameUpdateAndRender
cl %CommonCompilerFlags% ..\handmade\code\win32_startfile.cpp /link %CommonLinkerFlags%

popd

