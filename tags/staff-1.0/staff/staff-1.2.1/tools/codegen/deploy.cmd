set projectname=staff_codegen

set deploydir=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set bindir=%deploydir%\bin
set templatedir=%bindir%\template

if not EXIST %bindir% mkdir %bindir%
if not EXIST %templatedir% mkdir %templatedir%

xcopy /Y /E /S template\*.* %templatedir%\
xcopy /Y /S debug\*.exe %bindir%

set outdir=%STAFF_HOME%\bin
if not EXIST %outdir% mkdir %outdir%
xcopy /Y /S debug\*.exe %outdir%
if not EXIST %outdir%\template mkdir %outdir%\template
xcopy /Y /S template %outdir%\template
