setlocal
SET STAFF_HOME=%cd%\..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%\staff
set PATH=%cd%\..\..\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;%STAFF_HOME\lib%;%STAFF_HOME%\bin;%PATH%
cd src
staff_codegen -u -tclient Echo.h
endlocal
