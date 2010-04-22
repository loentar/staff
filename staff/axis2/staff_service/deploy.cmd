set projectname=staff_axis2_service

set deploydir=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set libdir=%deploydir%\axis2\services\staff

if not EXIST %libdir% mkdir %libdir%

if exist Debug (
  xcopy /Y /S debug\*.dll %libdir%
  xcopy /Y /S debug\*.dll %AXIS2C_HOME%\services\staff\
) else (
  if exist Release (
    xcopy /Y /S Release\*.dll %libdir%
    xcopy /Y /S Release\*.dll %AXIS2C_HOME%\services\staff\
  )
)


xcopy /Y /S config\services.xml %libdir%

xcopy /Y /S config\services.xml %AXIS2C_HOME%\services\staff\
