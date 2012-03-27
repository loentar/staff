set projectname=optional_client

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

set deploydir=%cd%\..\..\..\deploy\win_%arch%
set exdir=%deploydir%\staff\samples\bin
if not EXIST %exdir% mkdir %exdir%
xcopy /Y /S %target%*.exe %exdir%
