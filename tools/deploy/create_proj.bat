@echo off
setlocal
echo Creating project %1
if exist "%1" (
   echo Project %1 already exists. Skipping
   exit /B 0
)
mkdir "%1"
copy "opengl_lessons\package\project_template\main.cpp" "%1\main.cpp"

(for /f "delims=" %%i in (opengl_lessons\package\project_template\project_template.sln) do (
    set "line=%%i"
    setlocal enabledelayedexpansion
    set "line=!line:project_template=%1!"
    echo(!line!
    endlocal
))>"%1\%1.sln"
copy "opengl_lessons\package\project_template\project_template.vcxproj" "%1\%1.vcxproj"
copy "opengl_lessons\package\project_template\project_template.vcxproj.filters" "%1\%1.vcxproj.filters"
copy "opengl_lessons\package\project_template\project_template.vcxproj.user.template" "%1\%1.vcxproj.user"
echo Done creating project
