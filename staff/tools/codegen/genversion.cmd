@echo off

echo $WCREV$ >%TEMP%\staff-svnrev-template
SubWCRev ..\..\.. %TEMP%\staff-svnrev-template %TEMP%\staff-svnrev >nul
for /F "" %%i in (%TEMP%\staff-svnrev) do set REVISION=%%i

del %TEMP%\staff-svnrev-template %TEMP%\staff-svnrev

if exist src\version.h del src\version.h
for /F "delims== tokens=1,2" %%i in (..\..\..\VERSION) do @echo #define %%i "%%j" >> src\version.h

echo #define VERSION_FULL LIBRARY_VERSION ##"-r%REVISION%" >> src\version.h
