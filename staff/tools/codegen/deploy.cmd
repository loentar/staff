set projectname=codegen

set target=%1%
if "%target%" == "" (
  echo.
  echo Error: Target is not set.
  exit 1
)

set deploydir=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set bindir=%deploydir%\staff\bin
set templatedir=%bindir%\template

if not EXIST %bindir% mkdir %bindir%
if not EXIST %templatedir% mkdir %templatedir%

xcopy /Y /E /S template\*.* %templatedir%\
xcopy /Y /S %target%\*.exe %bindir%
