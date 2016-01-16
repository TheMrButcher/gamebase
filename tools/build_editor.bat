@echo off
setlocal

set EDITOR_SLN_PATH=%~dp0\..\src\design_editor\design_editor.sln
echo Starting build of %EDITOR_SLN_PATH%
set VS_BAT_PATH=%VS100COMNTOOLS%..\..\VC\vcvarsall.bat
if exist "%VS_BAT_PATH%" (
   call "%VS_BAT_PATH%" x86
   msbuild "%EDITOR_SLN_PATH%" /p:Configuration=Release /p:Platform=Win32 /t:Clean,Build
) else (
   echo Error! Can't find Visual Studio variables batch-file
)