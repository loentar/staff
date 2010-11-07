setlocal
SET STAFF_HOME=%cd%\..\..\staff\deploy\win_%PROCESSOR_ARCHITECTURE%\staff
set PATH=%cd%\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;%cd%\..\deploy\win_%PROCESSOR_ARCHITECTURE%\staff\lib;%STAFF_HOME%\lib;%STAFF_HOME%\bin;%PATH%
cd src
staff_codegen -u -tcomponent DataAccessService.h
staff_codegen -u -twsdl DataAccessService.h
endlocal
