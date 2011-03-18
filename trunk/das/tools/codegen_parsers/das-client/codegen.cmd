setlocal
SET STAFF_HOME=%cd%\..\..\..\..\staff\deploy\win_%PROCESSOR_ARCHITECTURE%\staff
set PATH=%cd%\..\..\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;%cd%\..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%\bin;%STAFF_HOME%\bin;%PATH%
cd src
staff_codegen -u -tclient DataAccessService.h
endlocal
