set projectname=client

set target=%1%
if "%target%" == "" (
  echo.
  echo Error: Target is not set.
  exit 1
)

set deploydir=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set incdir=%deploydir%\staff\include\staff\%projectname%
set libdir=%deploydir%\staff\lib

if not EXIST %incdir% mkdir %incdir%
if not EXIST %libdir% mkdir %libdir%

xcopy /Y /S src\*.h %incdir%
xcopy /Y /S src\*.hpp %incdir%
xcopy /Y /S %target%\*.lib %libdir%
xcopy /Y /S %target%\*.dll %libdir%
