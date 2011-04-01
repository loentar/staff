setlocal
SET STAFF_HOME=%cd%\..\..\..\..\staff\deploy\win_%PROCESSOR_ARCHITECTURE%\staff
set PATH=%cd%\..\..\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;%STAFF_HOME%\bin;%STAFF_HOME%\lib;%PATH%
cd src
staff_codegen -u -tclient DataAccessService.h
endlocal
