set projectname=client_blocking_non_blocking

set target=%1%
if "%target%" == "" (
  echo.
  echo Error: Target is not set.
  exit 1
)

set deploydir=%cd%\..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set exdir=%deploydir%\staff\samples\bin
if not EXIST %exdir% mkdir %exdir%
xcopy /Y /S %target%\*.exe %exdir%
