setlocal
set arch=%1%
if "%arch%" == "" (
  echo.
  echo Error: Arch is not set.
  exit 1
)
set PATH=%cd%\%STAFF_HOME\lib%;%STAFF_HOME%\bin;%PATH%
cd src

if not exist Users.h goto gen_das
if not exist Groups.h goto gen_das

goto skip_das

:gen_das
staff_codegen -pdas -tinterface staff.das.samples.Users staff.das.samples.Groups 

:skip_das

staff_codegen -u -tclient Users.h Groups.h
endlocal
