@echo off
setlocal
set SCRIPT_PATH=%~dp0.
if exist "%SCRIPT_PATH%\opengl_lessons.zip" (
   echo Deploying package
   if exist opengl_lessons (
      echo Package opengl_lessons already exists. Removing it
      rmdir /q /S opengl_lessons
   )
   if exist tmp (
      rmdir /q /S tmp
   )
   if exist opengl_lessons.zip (
      del opengl_lessons.zip
   )
   copy "%SCRIPT_PATH%\opengl_lessons.zip" opengl_lessons.zip
   call :UnZipFile tmp opengl_lessons.zip
   del opengl_lessons.zip
   if exist tmp\opengl_lessons-master (
      move tmp\opengl_lessons-master opengl_lessons
      rmdir /q /S tmp
   ) else (
      rename tmp opengl_lessons
   )
   call "opengl_lessons\tools\load_contrib.bat"
   call "opengl_lessons\tools\build.bat"
   echo Done deploying package
) else (
   echo Error! Can't find opengl_lessons.zip!
)
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