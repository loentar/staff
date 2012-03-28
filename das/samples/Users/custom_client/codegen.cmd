setlocal
::set arch=%1%
::if "%arch%" == "" (
::  echo.
::  echo Error: Arch is not set.
::  exit 1
::)
::set PATH=%cd%\%STAFF_HOME\lib%;%STAFF_HOME%\bin;%PATH%
cd src

staff_codegen -u -tclient DataAccessService.h
endlocal
