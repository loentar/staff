set projectname=calc

set deploydir=%cd%\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set componentdir=%deploydir%\components\%projectname%

if not EXIST %componentdir% mkdir %componentdir%
if not EXIST %STAFF_HOME%\components\%projectname% mkdir %STAFF_HOME%\components\%projectname%

echo "debug\*.dll => %componentdir%"
xcopy /Y /S debug\*.dll %componentdir%

echo "debug\*.dll => %STAFF_HOME%\components\%projectname%"
xcopy /Y /S debug\*.dll %STAFF_HOME%\components\%projectname%
