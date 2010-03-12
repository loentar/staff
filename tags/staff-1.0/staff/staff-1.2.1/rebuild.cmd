@echo off

setlocal
if "%VSINSTALLDIR%" == "" (
  call "%VS80COMNTOOLS%..\..\VC\vcvarsall.bat"
  if not ERRORLEVEL 0 (
    endlocal
    echo Error: can't detect vs8 install dir. Try to run this file from "Visual Studio 2005 Command Prompt" >2
    exit 1
  )
)

devenv /rebuild Debug staff.sln
pause
