@echo off

REM TODO - can we just build both with one exe?
REM /link -subsystem:windows,5.1
REM call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build
cl -MT -nologo -Gm- -GR- -EHa- -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7 -Fmwin32_handmade.map H:\code\win32_handmade.cpp /link -opt:ref -subsystem:windows,5.01 user32.lib gdi32.lib
popd