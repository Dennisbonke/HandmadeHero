@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

set CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4459 -DHANDMADE_INTERNAL=0 -DHANDMADE_SLOW=0 -DHANDMADE_WIN32=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref -subsystem:windows,5.02 user32.lib gdi32.lib winmm.lib

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build
cl %CommonCompilerFlags% H:\code\handmade.cpp -Fmhandmade.map -LD /link -incremental:no -opt:ref -PDB:handmade_%random%.pdb -EXPORT:GameUpdateAndRender -EXPORT:GameGetSoundSamples
cl %CommonCompilerFlags% H:\code\win32_handmade.cpp -Fmwin32_handmade.map /link %CommonLinkerFlags%
popd
