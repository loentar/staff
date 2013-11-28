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
set libdir=%deploydir%\staff\lib
set incdir=%deploydir%\staff\include\staff
set thisincdir=%incdir%\%projectname%

if not EXIST %libdir% mkdir %libdir%
xcopy /Y /S %target%*.dll %libdir%
xcopy /Y /S %target%*.lib %libdir%

if not EXIST %incdir% mkdir %incdir%
if not EXIST %thisincdir% mkdir %thisincdir%

xcopy /Y /S src\*.h %thisincdir%
