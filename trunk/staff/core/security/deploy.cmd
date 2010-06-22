set projectname=security

set target=%1%
if "%target%" == "" (
  echo.
  echo Error: Target is not set.
  exit 1
)

set deploydir=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set incdir=%deploydir%\staff\include\staff\%projectname%
set libdir=%deploydir%\staff\lib
set dbdir=%deploydir%\staff\db

if not EXIST %incdir% mkdir %incdir%
if not EXIST %libdir% mkdir %libdir%
if not EXIST %dbdir% mkdir %dbdir%

xcopy /Y /S src\*.h %incdir%

xcopy /Y /S db\*.db %dbdir%

xcopy /Y /S %target%\*.lib %libdir%
xcopy /Y /S %target%\*.dll %libdir%
