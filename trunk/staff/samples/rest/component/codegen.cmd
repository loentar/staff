setlocal
SET STAFF_HOME=%cd%\..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%\staff
set PATH=..\..\..\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;..\..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%\bin;%STAFF_HOME%\bin;%PATH%
cd src
staff_codegen -u -tcomponent Customers.h
staff_codegen -u -twsdl Customers.h
endlocal
