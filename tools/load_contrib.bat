@echo off
setlocal
echo Running %0
cd /d %~dp0
cd ..
if exist contrib (
   echo Removing old contrib directory
   rmdir /S contrib
)

if exist contrib exit /b

echo Creating %CD%\contrib
mkdir contrib

echo Creating %CD%\contrib\bin
Call tools\unzip.bat contrib\bin package\bin.zip

echo Creating %CD%\contrib\include
Call tools\unzip.bat contrib\include package\include.zip

echo Copying config
copy package\config.json contrib\bin\config.json

echo Done