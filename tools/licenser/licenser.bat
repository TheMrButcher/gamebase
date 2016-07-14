@echo off
setlocal
echo Licensing all files in directory: %1

cd /d %~dp0

for /R ..\..\%1 %%f in (*.h) do (
    call add_license.bat "%%f"
)

for /R ..\..\%1 %%f in (*.cpp) do (
    call add_license.bat "%%f"
)
