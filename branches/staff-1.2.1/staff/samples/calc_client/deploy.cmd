set projectname=staff_test_client2

set deploydir=%cd%\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set incdir=%deploydir%\include\staff
set libdir=%deploydir%\lib
set bindir=%deploydir%\bin
set thisincdir=%incdir%\%projectname%

if not EXIST %incdir% mkdir %incdir%
if not EXIST %libdir% mkdir %libdir%
if not EXIST %bindir% mkdir %bindir%


if exist Debug (
  xcopy /Y /S debug\*.exe %bindir%
) else (
  if exist Release (
    xcopy /Y /S Release\*.exe %bindir%
  )
)
