set projectname=common

set deploydir=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set incdir=%deploydir%\include\staff
set libdir=%deploydir%\lib
set bindir=%deploydir%\bin
set thisincdir=%incdir%\%projectname%

if not EXIST %incdir% mkdir %incdir%
if not EXIST %libdir% mkdir %libdir%
if not EXIST %bindir% mkdir %bindir%
if not EXIST %thisincdir% mkdir %thisincdir%

xcopy /Y /S src\*.h %thisincdir%

set outdir=%STAFF_HOME%\lib
if not EXIST %outdir% mkdir %outdir%

set outdir2=%STAFF_HOME%\include\staff\%projectname%
if not EXIST %outdir2% mkdir %outdir2%
xcopy /Y /S src\*.h %outdir2%

if exist Debug (
  xcopy /Y /S Debug\*.lib %libdir%
  xcopy /Y /S Debug\*.dll %libdir%
  xcopy /Y /S debug\*.lib %outdir%
  xcopy /Y /S debug\*.dll %outdir%
) else (
  if exist Release (
    xcopy /Y /S Release\*.lib %libdir%
    xcopy /Y /S Release\*.dll %libdir%
    xcopy /Y /S Release\*.lib %outdir%
    xcopy /Y /S Release\*.dll %outdir%
  )
)
