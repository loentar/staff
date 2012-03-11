@rem start this file to compile project
@echo off

setlocal
set target=%1%
set build=%2%
set cwd=%cd%
set buildlog=%cwd%\build.log

if '%target%' == '' (
  echo.
  echo Error: Target is not set.
  goto errhelp
)

if not '%target%' == 'Release' (
if not '%target%' == 'Debug' (
if not '%target%' == '"Release|Win32"' (
if not '%target%' == '"Debug|Win32"' (
if not '%target%' == '"Release|x64"' (
if not '%target%' == '"Debug|x64"' (
  echo.
  echo Error: Invalid target set.
  goto errhelp
)
)
)
)
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
  set VSVARS="%VS90COMNTOOLS%..\..\VC\vcvarsall.bat"
  goto vsdetected
)

if not "%VS100COMNTOOLS%" == "" (
  set VSVERSION=2010
  set VSVARS="%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"
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

if %VSVERSION% equ 2005 goto skip_upgrade

  rem Upgrading solutions for Visual Studio
  cd rise
  devenv /upgrade rise.sln >%buildlog%
  if %ERRORLEVEL% gtr 0 (
    echo Failed to upgrade solution rise.sln >&2
    echo Please see build.log >&2
    goto errexit
  )
  cd ..\staff
  devenv /upgrade staff.sln >%buildlog%
  if %ERRORLEVEL% gtr 0 (
    echo Failed to upgrade solution staff.sln >&2
    echo Please see build.log >&2
    goto errexit
  )
  cd ..\das
  devenv /upgrade das.sln >%buildlog%
  if %ERRORLEVEL% gtr 0 (
    echo Failed to upgrade solution das.sln >&2
    echo Please see build.log >&2
    goto errexit
  )
  cd ..

:skip_upgrade


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
    echo Please see build.log >&2
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
    echo Please see build.log >&2
    goto errexit
)

echo staff %build%ing complete

rem ========== das ===========

echo %build%ing das...
echo %build%ing das... >>%buildlog%
cd ..\das

devenv /%build% %target% das.sln >>%buildlog%
if %ERRORLEVEL% gtr 0 (
    echo Error while building das >&2
    echo Please see build.log >&2
    goto errexit
)

if "%POSTGRESQL_HOME%" == "" (
    echo Skipping Postgresql provider compilation: POSTGRESQL_HOME is not set
    goto skip_postgres
)

  cd providers\postgres
  devenv /%build% %target% postgres.vcproj >>%buildlog%
  if %ERRORLEVEL% gtr 0 (
    echo Error while building das postgres provider >&2
    echo Please see build.log >&2
    goto errexit
  )
  cd ..\..
:skip_postgres


  if "%MYSQL_HOME%" == "" (
    echo Skipping MySql provider compilation: MYSQL_HOME is not set
    goto skip_mysql
  )

  cd providers\mysql
  devenv /%build% %target% mysql.vcproj >>%buildlog%
  if %ERRORLEVEL% gtr 0 (
    echo Error while building das mysql provider >&2
    echo Please see build.log >&2
    goto errexit
  )
  cd ..\..
:skip_mysql

echo das %build%ing complete
cd ..

if "%build%" == "rebuild" goto buildok
if "%build%" == "build" goto buildok

goto exit
:buildok

set arch=Win32
if '%target%' == '"Release|x64"' (
set arch=x64
)
if '%target%' == '"Debug|x64"' (
set arch=x64
)

echo.
echo.
echo Project successfuly built.
echo Start `install %arch%` to install project.
echo.
goto exit

:errhelp
echo.
echo Start this script as
echo  %0% TARGET BUILD
echo where TARGET is one of `Release`, `Debug`, `"Release|Win32"`, `"Debug|Win32"`, `"Release|x64"`, `"Debug|x64"`
echo and BUILD is `build`, `rebuild`, `clean` or empty ^(defaults is rebuild^)
goto errexit

:errexit
pause
set ERRORLEVEL=1
:exit
endlocal
