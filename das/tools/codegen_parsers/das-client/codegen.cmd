setlocal
set arch=%1%
if "%arch%" == "" (
  echo.
  echo Error: Arch is not set.
  exit 1
)
SET STAFF_HOME=%cd%\..\..\..\..\staff\deploy\win_%arch%\staff
set PATH=%cd%\..\..\..\..\rise\deploy\win_%arch%\lib;%STAFF_HOME%\bin;%STAFF_HOME%\lib;%PATH%
cd src
staff_codegen -u -tclient DataAccessService.h
endlocal
