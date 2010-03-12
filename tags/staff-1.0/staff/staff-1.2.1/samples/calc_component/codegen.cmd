setlocal
cd src
staff_codegen -u -tcomponent CalcService.h SubService.h
staff_codegen -u -tservicelocator Calculator.h
endlocal
