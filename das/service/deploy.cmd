set projectname=staff.das

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

set deploydir=%cd%\..\deploy\win_%arch%
set incdir=%deploydir%\staff\include\staff\das\service
set componentdir=%deploydir%\staff\components\%projectname%

if not EXIST %incdir% mkdir %incdir%
if not EXIST %componentdir% mkdir %componentdir%

xcopy /Y /S src\DataAccessService.h %incdir%
xcopy /Y /S src\*.wsdl %componentdir%\
xcopy /Y /S %target%\*.dll %componentdir%\

