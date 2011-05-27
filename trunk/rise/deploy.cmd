@echo off
setlocal

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

echo deploying rise...

set platform=win_%arch%
set pwd=%cd%
set deploydir=%pwd%\deploy\%platform%
set deployincdir=%deploydir%\include\rise
set deploylibdir=%deploydir%\lib

cd rise
for /D %%d in (*) do (
setlocal
  if not exist %deployincdir%\%%d mkdir %deployincdir%\%%d
  cd %%d
  for %%f in (*.h;*.hpp) do (
    copy /y %%f %deployincdir%\%%d >NUL
  )
endlocal
)

cd ..

if not exist %deploylibdir% mkdir %deploylibdir%
copy /y %target%\rise.dll %deploylibdir% >NUL
copy /y %target%\rise.lib %deploylibdir% >NUL

echo done.

endlocal
