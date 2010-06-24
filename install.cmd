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

set risedeploydir=%cd%\rise\deploy\win_%PROCESSOR_ARCHITECTURE%
set staffdeploydir=%cd%\staff\deploy\win_%PROCESSOR_ARCHITECTURE%

if not exist %STAFF_HOME% mkdir %STAFF_HOME%

echo installing rise...
xcopy /Y /S %risedeploydir% %STAFF_HOME% >install.log
if %ERRORLEVEL% gtr 0 (
    echo Failed to install rise >&2
    echo Please look at install.log >&2
    goto errexit
)

echo installing axis2/c staff modules and service...
xcopy /Y /S %staffdeploydir%\axis2 %AXIS2C_HOME% >>install.log
if %ERRORLEVEL% gtr 0 (
    echo Failed to install axis2 staff modules and service >&2
    echo Please look at install.log >&2
    goto errexit
)

echo installing staff...
xcopy /Y /S %staffdeploydir%\staff %STAFF_HOME% >>install.log
if %ERRORLEVEL% gtr 0 (
    echo Failed to install staff >&2
    echo Please look at install.log >&2
    goto errexit
)

echo installation succesful

goto exit
:errexit
pause
:exit
