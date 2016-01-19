@echo off

mkdir W:\build
pushd W:\build
cl -FAcs -Zi W:\code\win32_handmade.cpp user32.lib gdi32.lib
popd