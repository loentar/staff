set projectname=samples.simple

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
set componentdir=%deploydir%\staff\components\%projectname%

if not EXIST %componentdir% mkdir %componentdir%

xcopy /Y /S src\*.wsdl %componentdir%
xcopy /Y /S %target%\*.dll %componentdir%
