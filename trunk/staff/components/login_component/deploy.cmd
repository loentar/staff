set projectname=login

set deploydir=%cd%\..\deploy\win32
set componentdir=%deploydir%\components\%projectname%

if not EXIST %componentdir% mkdir %componentdir%

echo "debug\*.dll => %componentdir%"
xcopy /Y /S debug\*.dll %componentdir%\

echo "debug\*.dll => %STAFF_HOME%\components\%projectname%"
xcopy /Y /S debug\*.dll %STAFF_HOME%\components\%projectname%\

