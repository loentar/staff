setlocal
set arch=%1%
if "%arch%" == "" (
  echo.
  echo Error: Arch is not set.
  exit 1
)
SET STAFF_HOME=%cd%\..\..\..\deploy\win_%arch%\staff
set PATH=..\%STAFF_HOME%\bin;%STAFF_HOME%\lib;%PATH%
cd src
staff_codegen -u -tcomponent DepSvc1.h DepSvc2.h DepSvc3.h DepSvc4.h
staff_codegen -u -twsdl DepSvc1.h DepSvc2.h DepSvc3.h DepSvc4.h
endlocal
