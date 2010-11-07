set projectname=staffcgparser-cpp

set target=%1%
if "%target%" == "" (
  echo.
  echo Error: Target is not set.
  exit 1
)

set deploydir=%cd%\..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set libdir=%deploydir%\staff\lib\codegen\parsers
if not EXIST %libdir% mkdir %libdir%
xcopy /Y /S %target%\*.dll %libdir%
