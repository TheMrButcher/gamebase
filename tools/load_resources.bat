@echo off
setlocal
echo Running %0
cd /d %~dp0
cd ..
if exist resources\fonts (
   echo Removing old fonts directory
   rmdir /S resources\fonts
)

echo Unzipping fonts
Call tools\unzip.bat resources\fonts package\fonts.zip

echo Done