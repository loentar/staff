@echo off
rem start this file to install project into %STAFF_HOME%

if "%STAFF_HOME%" == "" (
  echo Error: STAFF_HOME is not set
  goto errexit
)

if "%AXIS2C_HOME%" == "" (
  echo Error: AXIS2C_HOME is not set
  goto errexit
)

set staffdeploydir=%cd%\..\deploy\win_%PROCESSOR_ARCHITECTURE%

if not exist %STAFF_HOME%\samples mkdir %STAFF_HOME%\samples

echo installing samples...
xcopy /Y /S %staffdeploydir%\staff\samples %STAFF_HOME%\samples >>install.log
if %ERRORLEVEL% gtr 0 (
    echo Failed to install samples >&2
    echo Please see install.log >&2
    goto errexit
)

xcopy /Y /S %staffdeploydir%\staff\components %STAFF_HOME%\components >>install.log
if %ERRORLEVEL% gtr 0 (
    echo Failed to install samples >&2
    echo Please see install.log >&2
    goto errexit
)

echo installation succesful

goto exit
:errexit
pause
:exit
