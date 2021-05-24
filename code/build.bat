@echo off

set CommonCompilerFlags=-Od -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Oi -WX -W4 -wd4505 -wd4312 -wd4244 -wd4201 -wd4204 -wd4255 -wd4668 -wd4820 -wd4100 -wd4189 -wd4711 -wd4710 -wd4101 -wd4296 -wd4311 -wd4115 -wd4702 -wd4456 -wd4555 -wd4457 -wd4245 -wd4701 -FC -Zi
set CommonCompilerFlags=-DDEVELOPMENT=1 -DPROFILER=1 %CommonCompilerFlags%
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib opengl32.lib winmm.lib dsound.lib

set ReleaseCompilerFlags=-MTd -O2 -Oi

if not exist ..\..\build mkdir ..\..\build
pushd ..\..\build

REM Debug 64-bit build
del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% -D_CRT_SECURE_NO_WARNINGS ..\game-training\code\win32_gt.cpp -Fmwin32_gt.map /link %CommonLinkerFlags%

REM Release 64-bit build:
REM cl %ReleaseCompilerFlags% -D_CRT_SECURE_NO_WARNINGS ..\game-training\code\win32_gt.cpp /link %CommonLinkerFlags%

popd

REM Compiler Flags:

REM Zi  : debug info (Z7 older debug format for complex builds)
REM Zo  : More debug info for optimized builds
REM FC  : Full path on errors
REM Oi  : Always do intrinsics with you can
REM Od  : No optimizations
REM O2  : Full optimizations
REM MT  : Use the c static lib instead of searching for dll at run-time
REM MTd : Sabe as MT but using the debug version of CRT
REM GR- : Turn off C++ run-time type info
REM Gm- : Turn off incremental build
REM EHa-: Turn off exception handling
REM WX  : Treat warning as errors
REM W4  : Set Warning level to 4 (Wall to all levels)
REM wd  : Ignore warning
REM fp:fast    : Ignores the rules in some cases to optimize fp operations
REM Fmfile.map : Outputs a map file (mapping of the functions on the exr)

REM Linker Options:

REM subsystem:windows,5.1 : Make exe compatible with Windows XP (only works on x86)
REM opt:ref               : Don't put unused things in the exe
REM incremental:no        : Don't need to do incremental builds
REM LD                    : Build a dll
REM PDB:file.pdb          : Change the .pdb's path