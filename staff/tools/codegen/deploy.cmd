set projectname=codegen

set deploydir=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set incdir=%deploydir%\include\staff
set bindir=%deploydir%\bin
set templatedir=%bindir%\template
set thisincdir=%incdir%\%projectname%

if not EXIST %incdir% mkdir %incdir%
if not EXIST %bindir% mkdir %bindir%
if not EXIST %templatedir% mkdir %templatedir%
if not EXIST %thisincdir% mkdir %thisincdir%

xcopy /Y /E /S template\*.* %templatedir%\
xcopy /Y /S src\CodegenParser.h %thisincdir%
xcopy /Y /S src\Interface.h %thisincdir%

set outdir=%STAFF_HOME%\bin
if not EXIST %outdir% mkdir %outdir%
if not EXIST %outdir%\template mkdir %outdir%\template
xcopy /Y /S template %outdir%\template

set outdir2=%STAFF_HOME%\include\staff\%projectname%
if not EXIST %outdir2% mkdir %outdir2%
xcopy /Y /S src\CodegenParser.h %outdir2%
xcopy /Y /S src\Interface.h %outdir2%


if exist Debug (
  xcopy /Y /S debug\*.exe %bindir%
  xcopy /Y /S debug\*.exe %outdir%
) else (
  if exist Release (
    xcopy /Y /S Release\*.exe %bindir%
    xcopy /Y /S Release\*.exe %outdir%
  )
)
