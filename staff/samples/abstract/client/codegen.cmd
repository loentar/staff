setlocal
set arch=%1%
if "%arch%" == "" (
  echo.
  echo Error: Arch is not set.
  exit 1
)
SET STAFF_HOME=%cd%\..\..\..\deploy\win_%arch%\staff
set PATH=..\%STAFF_HOME%\lib;%STAFF_HOME%\bin;%PATH%
cd src
staff_codegen -u -tclient Transport.h Transport2.h
endlocal
