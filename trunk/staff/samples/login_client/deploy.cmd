set projectname=staff_test_login

set deploydir=%cd%\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set bindir=%deploydir%\bin

if not EXIST %bindir% mkdir %bindir%



if exist Debug (
  xcopy /Y /S debug\*.exe %bindir%
) else (
  if exist Release (
    xcopy /Y /S Release\*.exe %bindir%
  )
)
