@echo off
setlocal
echo Running %0
cd /d %~dp0
cd ..

for /f "tokens=*" %%p in ('dir /a:d /b src\gltests') do (
    if exist src\gltests\%%p\%%p.vcxproj (
        echo Copying debug environment for project gltests\%%p
        copy package\project.vcxproj.user.template src\gltests\%%p\%%p.vcxproj.user
    )
)

echo Copying debug environment for project design_editor
copy package\project.vcxproj.user.template src\design_editor\design_editor.vcxproj.user
