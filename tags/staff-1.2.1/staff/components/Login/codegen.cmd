setlocal
PATH=%PATH%;..\..\..\..\rise\deploy\win_%PROCESSOR_ARCHITECTURE%\lib;..\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%\bin
cd src
staff_codegen -u -tcomponent Login.h
endlocal