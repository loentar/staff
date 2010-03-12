@echo off
setlocal

echo deploying rise...

set platform=win_%PROCESSOR_ARCHITECTURE%
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
if exist Debug (
  copy /y Debug\rise.dll %deploylibdir% >NUL
  copy /y Debug\rise.lib %deploylibdir% >NUL
) else (
  if exist Release (
    copy /y Release\rise.dll %deploylibdir% >NUL
    copy /y Release\rise.lib %deploylibdir% >NUL
  )
)

echo done.

endlocal
