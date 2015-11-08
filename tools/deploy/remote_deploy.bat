@echo off
setlocal
if exist opengl_lessons.zip (
   del opengl_lessons.zip
)

echo Downloading opengl_lessons.zip
powershell -command "(new-object System.Net.WebClient).DownloadFile('https://github.com/TheMrButcher/opengl_lessons/archive/master.zip', 'opengl_lessons.zip')"

if exist deploy (
   rmdir /q /S deploy
)

if exist tmp (
   rmdir /q /S tmp
)

echo Preparing deploy...

call :UnZipFile tmp opengl_lessons.zip
move tmp\opengl_lessons-master\tools\deploy deploy
move opengl_lessons.zip deploy\opengl_lessons.zip
rmdir /q /S tmp

echo Creating deploy_here.bat
echo call deploy\deploy.bat %~dp0 > deploy_here.bat

echo Deploying...
call deploy_here.bat

exit /B 0

:UnZipFile <ExtractTo> <newzipfile>
echo Unzipping %2 to %1

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

set vbs="_.vbs"
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
if not exist %1 (
   echo Error while unzipping %2
)
if exist %vbs% del /f /q %vbs%

exit /B 0