@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DHANDMADE_INTERNAL=0 -DHANDMADE_SLOW=0 -DHANDMADE_WIN32=1 -FC -Z7 -Fmwin32_handmade.map
set CommonLinkerFlags=-opt:ref -subsystem:windows,5.01 user32.lib gdi32.lib winmm.lib

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build
cl %CommonCompilerFlags% H:\code\win32_handmade.cpp /link %CommonLinkerFlags%
popd