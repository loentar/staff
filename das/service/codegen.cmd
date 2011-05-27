setlocal
set arch=%1%
if "%arch%" == "" (
  echo.
  echo Error: Arch is not set.
  exit 1
)
SET STAFF_HOME=%cd%\..\..\staff\deploy\win_%arch%\staff
set PATH=%cd%\..\..\rise\deploy\win_%arch%\lib;%cd%\..\deploy\win_%arch%\staff\lib;%STAFF_HOME%\lib;%STAFF_HOME%\bin;%PATH%
cd src
staff_codegen -u -tcomponent DataAccessService.h
staff_codegen -u -twsdl DataAccessService.h
endlocal
