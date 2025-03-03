@echo off

IF NOT EXIST temp mkdir temp
for /f %%a in ('wmic os get osarchitecture ^| find /i "bit"') do set "bits=%%a"

echo %bits%

REM -Z7
set CompilerFlags=-nologo -Gm- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -FC -std:c++20 -I include -I include\imgui -I include\sokol
set LinkerFlags=-incremental:no -opt:ref 
echo
echo %time%

del *.pdb > NUL 2> NUL
del *.obj > NUL 2> NUL
del *.lib > NUL 2> NUL
del *.dll> NUL 2> NUL

for /f %%a in ('wmic os get osarchitecture ^| find /i "bit"') do set "bits=%%a"
set arch="%bits%"

if %arch%=="ARM" (
    echo building for arm
    REM Arm 64
    REM
    REM link without Zi, or it'll warn you about the lack of an obj file
    REM uncomment to generate linker
    REM cl %CompilerFlags% -c -I include\sokol -I include\imgui lib\win_arm64\sokol.cpp -Fo:sokol.obj
    REM lib /nologo /out:lib\win_arm64\sokol.lib sokol.obj

    cl %CompilerFlags% src/main.cpp -Fo: temp/main.obj -Fe: temp/tradewinds.exe /link %LinkerFlags% lib\win_arm64\sokol.lib 
)

if %arch%=="64-bit" (
    echo building for intel
    REM
    REM intel x64
    REM For recreating the link to sokol
    REM Build without Zi
    REM cl %CompilerFlags% -c -I include -I include\imgui lib\win_x64\sokol.cpp -Fo:sokol.obj
    REM lib /nologo /out:lib\win_x64\sokol.lib sokol.obj
    cl %CompilerFlags% src/main.cpp -Fo: temp/main.obj -Fe: temp/tradewinds.exe /link %LinkerFlags% lib\win_x64\sokol.lib 
)


rem older:

REM not working dll generation
REM cl %CompilerFlags% -I include src\main.c -LD /link -incremental:no -opt:ref -PDB:seth.pdb -EXPORT:game_loop %LinkerFlags%
REM cl %CompilerFlags% -I include src\renderer.c -Fe:temp\main.exe -link main.lib

echo %time%

if errorlevel 1 exit /b 1
temp\tradewinds.exe
