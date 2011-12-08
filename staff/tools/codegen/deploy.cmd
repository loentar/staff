set projectname=codegen

set target=%1%
if "%target%" == "" (
  echo.
  echo Error: Target is not set.
  exit 1
)

set arch=%2%
if "%arch%" == "" (
  echo.
  echo Error: Arch is not set.
  exit 1
)

set deploydir=%cd%\..\..\deploy\win_%arch%
set bindir=%deploydir%\staff\bin
set templatedir=%deploydir%\staff\lib\codegen\templates

if not EXIST %bindir% mkdir %bindir%
if not EXIST %templatedir% mkdir %templatedir%

xcopy /Y /E /S templates\*.* %templatedir%\
xcopy /Y /S %target%\*.exe %bindir%
