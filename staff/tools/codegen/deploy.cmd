set projectname=codegen

set target=%1%
if "%target%" == "" (
  echo.
  echo Error: Target is not set.
  exit 1
)

set deploydir=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set incdir=%deploydir%\staff\include\staff
set bindir=%deploydir%\staff\bin
set templatedir=%bindir%\template
set thisincdir=%incdir%\%projectname%

if not EXIST %incdir% mkdir %incdir%
if not EXIST %bindir% mkdir %bindir%
if not EXIST %templatedir% mkdir %templatedir%
if not EXIST %thisincdir% mkdir %thisincdir%

xcopy /Y /E /S template\*.* %templatedir%\
xcopy /Y /S src\CodegenParser.h %thisincdir%
xcopy /Y /S src\Interface.h %thisincdir%
xcopy /Y /S %target%\*.exe %bindir%
