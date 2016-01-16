@echo off
setlocal
echo Running %0
cd /d %~dp0
cd ..

echo Copying editor design
xcopy /S /Y resources\designs\design_editor contrib\bin

echo Done