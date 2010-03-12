setlocal
call vcvars32

del /s /q Debug
vcbuild rise.vcproj Debug
if not ERRORLEVEL 0 goto exit

:exit
endlocal
