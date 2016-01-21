@echo off

IF NOT EXIST ..\..\build mkdir W:\build
pushd W:\build
cl -FAcs -Zi W:\code\win32_voidt.cpp user32.lib gdi32.lib
popd