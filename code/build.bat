@echo off

SET CommonCompilerFlags=-MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -DDEBUG=1 -DINTERNAL=1 -FAcs -Z7 -Fm
SET CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

IF NOT EXIST ..\..\build mkdir W:\build
pushd W:\build

REM 32-bit build
REM cl %CommonCompilerFlags% W:\code\win32_voidt.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% W:\code\voidt.cpp -LD /link -incremental:no -PDB:voidt_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb -EXPORT:GameUpdateAndRender -EXPORT:GameGetSoundSamples
cl %CommonCompilerFlags% W:\code\win32_voidt.cpp /link %CommonLinkerFlags%

popd

:: note when building for devices like windows xp with -subsystem:windows5.1 ; if you're building x64 machine code this should be windows,5.02 instead of 5.1