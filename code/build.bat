@echo off

set CommonCompilerFlags=-O2 -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -FC -Z7
set CommonCompilerFlags=-DDEVELOPMENT=1 %CommonCompilerFlags%
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib opengl32.lib winmm.lib

set ReleaseCompilerFlags=-MTd -O2 -Oi

if not exist ..\..\build mkdir ..\..\build
pushd ..\..\build

REM Debug 64-bit build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% -D_CRT_SECURE_NO_WARNINGS ..\game-training\code\win32_gt.cpp -Fmwin32_gt.map /link %CommonLinkerFlags%

REM Release 64-bit build:
REM cl %ReleaseCompilerFlags% -D_CRT_SECURE_NO_WARNINGS ..\game-training\code\win32_gt.cpp /link %CommonLinkerFlags%

popd
