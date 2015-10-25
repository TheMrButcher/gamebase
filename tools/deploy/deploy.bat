@echo off
setlocal
set DEPLOY_PATH=%~1.
cd %DEPLOY_PATH%
echo Deploying opengl_lessons in %DEPLOY_PATH%
set SCRIPT_PATH=%~dp0.
echo Deploy script is located in %SCRIPT_PATH%
if exist "%SCRIPT_PATH%\deploy_steps.bat" (
   echo Detected specific deploy steps batch-file. Running...
   call "%SCRIPT_PATH%\deploy_steps.bat"
) else (
   echo No specific deploy steps. Running default deployment
   call "%SCRIPT_PATH%\deploy_package.bat"
)
