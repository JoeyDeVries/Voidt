@echo off

IF NOT EXIST ..\..\build mkdir W:\build
pushd W:\build
cl -MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DDEBUG=1 -DINTERNAL=1 -FAcs -Z7 -Fm W:\code\win32_voidt.cpp /link -opt:ref  -subsystem:windows,5.02 user32.lib gdi32.lib
popd


:: note when building for devices like windows xp with -subsystem:windows5.1 ; if you're building x64 machine code this should be windows,5.02 instead of 5.1