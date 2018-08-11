@echo off

REM -wd4456 is debug!
set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4459 -wd4456 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7 -Fmwin32_handmade.map
set CommonLinkerFlags=-opt:ref -subsystem:windows,5.02 user32.lib gdi32.lib winmm.lib

REM TODO - can we just build both with one exe?
REM /link -subsystem:windows,5.1
REM call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build
cl %CommonCompilerFlags% H:\code\win32_handmade.cpp /link %CommonLinkerFlags%
popd
