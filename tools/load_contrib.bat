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
Call :UnZipFile contrib\bin package\bin.zip

echo Creating %CD%\contrib\include
Call :UnZipFile contrib\include package\include.zip

copy package\config.json contrib\bin\config.json

exit /b

:UnZipFile <ExtractTo> <newzipfile>
if exist "%ProgramFiles(x86)%\7-Zip\7z.exe" (
   echo Detected 7zip, using it
   "%ProgramFiles(x86)%\7-Zip\7z.exe" x -y -o"%1" "%2" > nul
   goto :UnZipEnd
)

if exist "%ProgramFiles%\7-Zip\7z.exe" (
   echo Detected 7zip, using it
   "%ProgramFiles%\7-Zip\7z.exe" x -y -o"%1" "%2" > nul
   goto :UnZipEnd
)

set vbs="tools\_.vbs"
if exist %vbs% del /f /q %vbs%
>%vbs%  echo Set fso = CreateObject("Scripting.FileSystemObject")
>>%vbs% echo dstDir = fso.GetAbsolutePathName("%1")
>>%vbs% echo srcZip = fso.GetAbsolutePathName("%2")
>>%vbs% echo If NOT fso.FileExists(srcZip) Then
>>%vbs% echo Wscript.Quit
>>%vbs% echo End If
>>%vbs% echo If NOT fso.FolderExists(dstDir) Then
>>%vbs% echo fso.CreateFolder(dstDir)
>>%vbs% echo End If
>>%vbs% echo Set objShell = CreateObject("Shell.Application")
>>%vbs% echo Set FilesInZip=objShell.NameSpace(srcZip).Items()
>>%vbs% echo intOptions = 256
>>%vbs% echo objShell.NameSpace(dstDir).CopyHere FilesInZip, intOptions
>>%vbs% echo Set fso = Nothing
>>%vbs% echo Set objShell = Nothing
cscript //nologo %vbs%

:UnZipEnd
if not exist %1 echo Error while unzipping %2
if exist %vbs% del /f /q %vbs%