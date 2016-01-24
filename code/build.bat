@echo off

SET CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DDEBUG=1 -DINTERNAL=1 -FAcs -Z7 -Fm
SET CommonLinkerFlags= -opt:ref user32.lib gdi32.lib

IF NOT EXIST ..\..\build mkdir W:\build
pushd W:\build

REM 32-bit build
REM cl %CommonCompilerFlags% W:\code\win32_voidt.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
cl %CommonCompilerFlags% W:\code\win32_voidt.cpp /link %CommonLinkerFlags%

popd

:: note when building for devices like windows xp with -subsystem:windows5.1 ; if you're building x64 machine code this should be windows,5.02 instead of 5.1