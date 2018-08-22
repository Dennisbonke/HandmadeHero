@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

REM -wd4456 is debug!
set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4459 -wd4456 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref -subsystem:windows,5.02 user32.lib gdi32.lib winmm.lib

REM TODO - can we just build both with one exe?
REM /link -subsystem:windows,5.1
REM call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% H:\code\handmade.cpp -Fmhandmade.map -LD /link -incremental:no -opt:ref -PDB:handmade_%random%.pdb -EXPORT:GameUpdateAndRender -EXPORT:GameGetSoundSamples
cl %CommonCompilerFlags% H:\code\win32_handmade.cpp -Fmwin32_handmade.map /link %CommonLinkerFlags%
popd
