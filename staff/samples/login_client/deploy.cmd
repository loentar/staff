set projectname=login_client

set deploydir=%cd%\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set exdir=%deploydir%\bin
if not EXIST %exdir% mkdir %exdir%


set exdir2=%STAFF_HOME%\samples\bin
if not EXIST %exdir2% mkdir %exdir2%

if exist Debug (
  xcopy /Y /S debug\*.exe %exdir%
  xcopy /Y /S debug\*.exe %exdir2%
) else (
  if exist Release (
    xcopy /Y /S Release\*.exe %exdir%
    xcopy /Y /S Release\*.exe %exdir2%
  )
)
