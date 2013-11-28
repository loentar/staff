setlocal
set arch=%1%
if "%arch%" == "" (
  echo.
  echo Error: Arch is not set.
  exit 1
)
if "%STAFF_HOME%" == "" (
  echo.
  echo Error: STAFF_HOME is not set.
  exit 1
)
set PATH=%STAFF_HOME%\lib;%STAFF_HOME%\bin;%PATH%
%STAFF_HOME%\bin\staff_codegen -u -tcomponent -csrc Accounts.h
endlocal
