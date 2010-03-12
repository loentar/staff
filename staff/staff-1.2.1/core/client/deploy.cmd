set projectname=client

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
xcopy /Y /S src\*.hpp %thisincdir%
xcopy /Y /S debug\*.lib %libdir%
xcopy /Y /S debug\*.dll %libdir%

set outdir=c:\ws\staff\lib
if not EXIST %outdir% mkdir %outdir%
xcopy /Y /S debug\*.lib %outdir%
xcopy /Y /S debug\*.dll %outdir%
