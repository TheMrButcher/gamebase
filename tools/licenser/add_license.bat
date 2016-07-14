@echo off
setlocal

echo Adding license to file "%1"
copy %1 temp.txt
copy license_code.txt %1
type temp.txt >>%1
del temp.txt

