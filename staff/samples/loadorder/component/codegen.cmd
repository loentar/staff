setlocal
SET STAFF_HOME=%cd%\..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%\staff
set PATH=..\..\..\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;%STAFF_HOME%\bin;%STAFF_HOME%\lib;%PATH%
cd src
staff_codegen -u -tcomponent DepSvc1.h DepSvc2.h DepSvc3.h DepSvc4.h
staff_codegen -u -twsdl DepSvc1.h DepSvc2.h DepSvc3.h DepSvc4.h
endlocal
