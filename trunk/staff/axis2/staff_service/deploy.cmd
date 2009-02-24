set projectname=staff_axis2_service

set deploydir=%cd%\..\deploy\win32
set libdir=%deploydir%\axis2\services\staff

if not EXIST %libdir% mkdir %libdir%

xcopy /Y /S debug\*.dll %libdir%
xcopy /Y /S config\services.xml %libdir%
xcopy /Y /S config\staff.xml %deploydir%

xcopy /Y /S debug\*.dll %AXIS2C_HOME%\services\staff\
xcopy /Y /S config\services.xml %AXIS2C_HOME%\services\staff\
xcopy /Y /S config\staff.xml %STAFF_HOME%\