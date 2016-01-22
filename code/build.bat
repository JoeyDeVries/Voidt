@echo off

IF NOT EXIST ..\..\build mkdir W:\build
pushd W:\build
cl -DDEBUG=1 -DINTERNAL=1 -FAcs -Zi W:\code\win32_voidt.cpp user32.lib gdi32.lib
popd