@rem start this file to compile project
@echo off

setlocal
set target=%1%
set build=%2%
set cwd=%cd%
set buildlog=%cwd%\build.log

if "%target%" == "" (
  echo.
  echo Error: Target is not set.
  goto errhelp
)

if not "%target%" == "Release" (
if not "%target%" == "Debug" (
  echo.
  echo Error: Invalid target set.
  goto errhelp
)
)

if "%build%" == "" (
  set build=rebuild
)

if not "%VS80COMNTOOLS%" == "" (
  set VSVERSION=2005
  set VSVARS="%VS80COMNTOOLS%..\..\VC\vcvarsall.bat"
  goto vsdetected
)

if not "%VS90COMNTOOLS%" == "" (
  set VSVERSION=2008
  set VSVARS="%VS90COMNTOOLS%\vcvarsall.bat"
  goto vsdetected
)

if not "%VS100COMNTOOLS%" == "" (
  set VSVERSION=2010
  set VSVARS="%VS100COMNTOOLS%\vcvarsall.bat"
  goto vsdetected
)

echo Error: Cannot detect visual studio! >&2
goto errexit
:vsdetected

echo Detected Visual Studio %VSVERSION%

if "%VSINSTALLDIR%" == "" (
  call %VSVARS%
  if not ERRORLEVEL 0 (
    echo Error: Failed to set Visual Studio %VSVERSION% environment. >&2
    echo Try to run this file from ^"Visual Studio %VSVERSION% Command Prompt^" >&2
    goto errexit
  )
)

echo %build%ing log... >%buildlog%

if %VSVERSION% gtr 2005 (
  rem Upgrading solutions for Visual Studio
  cd rise
  devenv /upgrade rise.sln >%buildlog%
  if %ERRORLEVEL% gtr 0 (
    echo Failed to upgrade solution rise.sln >&2
    echo Please look at build.log >&2
    goto errexit
  )
  cd ..\staff
  devenv /upgrade staff.sln >%buildlog%
  if %ERRORLEVEL% gtr 0 (
    echo Failed to upgrade solution staff.sln >&2
    echo Please look at build.log >&2
    goto errexit
  )
  cd ..
)

echo %build%ing %target% version...

rem VS target
set ConfigurationName=target

rem ========== rise ===========

echo %build%ing rise...
echo %build%ing rise... >>%buildlog%
cd rise

devenv /%build% %target% rise.sln >>%buildlog%
if %ERRORLEVEL% gtr 0 (
    echo Error while building rise >&2
    echo Please look at build.log >&2
    goto errexit
)

echo rise %build%ing complete

rem ========== staff ===========

echo %build%ing staff...
echo %build%ing staff... >>%buildlog%
cd ..\staff

devenv /%build% %target% staff.sln >>%buildlog%
if %ERRORLEVEL% gtr 0 (
    echo Error while building staff >&2
    echo Please look at build.log >&2
    goto errexit
)

echo staff %build%ing complete

if "%build%" == "rebuild" goto buildok
if "%build%" == "build" goto buildok

goto exit
:buildok
echo.
echo.
echo Project successfuly built.
echo Start install.cmd to install project.
echo.
goto exit

:errhelp
echo.
echo Start this script as
echo  %0% TARGET BUILD
echo where TARGET is "Release" or "Debug"
echo and BUILD is "build", "rebuild", "clean" or empty ^(defaults is rebuild^)
goto errexit

:errexit
pause
:exit
endlocal
