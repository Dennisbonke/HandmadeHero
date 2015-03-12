@echo off

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build
cl -DHANDMADE_INTERNAL=1 -FC -Zi H:\code\win32_handmade.cpp user32.lib gdi32.lib
popd