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
set libdir=%deploydir%\staff\components\staff.das\providers\staff.das.Shell\

if not EXIST %libdir% mkdir %libdir%

xcopy /Y /S %target%*.dll %libdir%
