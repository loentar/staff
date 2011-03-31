setlocal
SET STAFF_HOME=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%\staff
set PATH=..\..\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;%STAFF_HOME%\bin;%STAFF_HOME%\lib\codegen\parsers;%PATH%
cd src
staff_codegen -u -tcomponent Wsdl.h
staff_codegen -u -twsdl Wsdl.h
endlocal
