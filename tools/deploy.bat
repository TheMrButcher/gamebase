@echo off
setlocal
set DEPLOY_PATH=%1
echo Deploying opengl_lessons in %DEPLOY_PATH%
set SCRIPT_PATH=%~dp0
echo Deploy script is located in %SCRIPT_PATH%
if exist "%SCRIPT_PATH%\deploy_steps.bat" (
   echo Detected specific deploy steps batch-file. Running...
   call "%SCRIPT_PATH%\deploy_steps.bat"
) else (
   echo No specific deploy steps. Running default deployment
   call :DeployPackage
)
exit /b 0

:DeployPackage
if exist "%SCRIPT_PATH%\openg_lessons.zip" (
   echo Deploying package
   if exist "%DEPLOY_PATH%\openg_lessons" (
      echo Package "%DEPLOY_PATH%\openg_lessons" already exists. Removing it
      rmdir /S contrib
   )
   call :UnZipFile "%DEPLOY_PATH%\openg_lessons" "%SCRIPT_PATH%\openg_lessons.zip"
   call "%DEPLOY_PATH%\openg_lessons\tools\load_contrib.bat"
   call "%DEPLOY_PATH%\openg_lessons\tools\build.bat"
   echo Done deploying package
) else (
   echo Error! Can't find openg_lessons.zip!
   exit /B 1
)
exit /B 0

:CreateProj <ProjName>
echo Creating project %DEPLOY_PATH%\%1
if exist "%DEPLOY_PATH%\%1" (
   echo Project %DEPLOY_PATH%\%1 already exists. Skipping
   exit /B 0
)
mkdir "%DEPLOY_PATH%\%1"
copy "%DEPLOY_PATH%\openg_lessons\package\project_template\main.cpp" "%DEPLOY_PATH%\%1\main.cpp"
copy "%DEPLOY_PATH%\openg_lessons\package\project_template\project_template.vcxproj" "%DEPLOY_PATH%\%1\%1.vcxproj"
copy "%DEPLOY_PATH%\openg_lessons\package\project_template\project_template.vcxproj.filters" "%DEPLOY_PATH%\%1\%1.vcxproj.filters"
copy "%DEPLOY_PATH%\openg_lessons\package\project_template\project_template.vcxproj.user" "%DEPLOY_PATH%\%1\%1.vcxproj.user"
echo Done creating project
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
if not exist %1 echo Error while unzipping %2
if exist %vbs% del /f /q %vbs%

exit /B 0

:DeployEnd