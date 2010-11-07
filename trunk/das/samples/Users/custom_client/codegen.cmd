setlocal
::set PATH=%cd%\..\..\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;%STAFF_HOME\lib%;%STAFF_HOME%\bin;%PATH%
cd src

staff_codegen -u -tclient DataAccessService.h
endlocal
