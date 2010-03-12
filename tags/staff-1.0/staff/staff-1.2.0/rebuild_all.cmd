setlocal
call vcvars32

cd staff_common
del /s /q Debug
vcbuild staffcommon.vcproj Debug
if not ERRORLEVEL 0 exit
cd ..

cd staff_axis2_module
del /s /q Debug
vcbuild staff_axis2_module.vcproj Debug

if not ERRORLEVEL 0 exit
cd ..

cd staff_axis2_service
del /s /q Debug
vcbuild staff_axis2_service.vcproj Debug

if not ERRORLEVEL 0 exit
cd ..

cd staff_client
del /s /q Debug
vcbuild staff_client.vcproj Debug
if not ERRORLEVEL 0 exit
cd ..

cd staff_service
del /s /q Debug
vcbuild staff_service.vcproj Debug
if not ERRORLEVEL 0 exit
cd ..

cd staff_codegen
del /s /q Debug
vcbuild staff_codegen.vcproj Debug
if not ERRORLEVEL 0 exit
cd ..

cd staff_test_client
del /s /q Debug
vcbuild staff_test_client.vcproj Debug
if not ERRORLEVEL 0 exit
cd ..

cd staff_test_client2
del /s /q Debug
vcbuild staff_test_client2.vcproj Debug
if not ERRORLEVEL 0 exit
cd ..

cd staff_test_service
del /s /q Debug
vcbuild staff_test_service.vcproj Debug
if not ERRORLEVEL 0 exit
cd ..

cd staff_test_service2
del /s /q Debug
vcbuild staff_test_service2.vcproj Debug
if not ERRORLEVEL 0 exit
cd ..

cd staff_test_service_client
del /s /q Debug
vcbuild staff_test_service_client.vcproj Debug
if not ERRORLEVEL 0 exit
cd ..
endlocal
