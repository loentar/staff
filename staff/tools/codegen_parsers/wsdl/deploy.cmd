set projectname=staffcgparser-wsdl

set deploydir=%cd%\..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set libdir=%deploydir%\lib\codegen\parsers
if not EXIST %libdir% mkdir %libdir%

set outdir=%STAFF_HOME%\lib\codegen\parsers
if not EXIST %outdir% mkdir %outdir%

if exist Debug (
  xcopy /Y /S debug\*.dll %libdir%
  xcopy /Y /S debug\*.dll %outdir%
) else (
  if exist Release (
    xcopy /Y /S Release\*.dll %libdir%
    xcopy /Y /S Release\*.dll %outdir%
  )
)
