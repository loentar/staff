setlocal
SET STAFF_HOME=%cd%\..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%\staff
set PATH=..\..\..\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;%STAFF_HOME%\bin;%STAFF_HOME%\lib;%PATH%
cd src
staff_codegen -u -tcomponent CalcService.h SubService.h
staff_codegen -u -twsdl CalcService.h SubService.h
endlocal
