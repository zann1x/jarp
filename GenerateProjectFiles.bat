@echo off
call ThirdParty\bin\premake\Windows\premake5.exe vs2019
if %errorlevel% neq 0 (
  pause
)
