@echo off
subst V: E:\handmade
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
set path V:\handmade\misc;%path%