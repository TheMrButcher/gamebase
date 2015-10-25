@echo off
setlocal

set GAMEBASE_SLN_PATH=%~dp0\..\src\gamebase\gamebase.sln
echo Starting build of %GAMEBASE_SLN_PATH%
set VS_BAT_PATH=%VS100COMNTOOLS%..\..\VC\vcvarsall.bat
if exist "%VS_BAT_PATH%" (
   call "%VS_BAT_PATH%" x86
   msbuild "%GAMEBASE_SLN_PATH%" /p:Configuration=Release /p:Platform=Win32 /t:Clean,Build
) else (
   echo Error! Can't find Visual Studio variables batch-file
)