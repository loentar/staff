set projectname=staff_axis2_module

set deploydir=%cd%\..\..\deploy\win_%PROCESSOR_ARCHITECTURE%
set libdir=%deploydir%\axis2\modules\staff

if not EXIST %libdir% mkdir %libdir%
if exist Debug (
  xcopy /Y /S Debug\*.dll %libdir%
  xcopy /Y /S Debug\*.dll %AXIS2C_HOME%\modules\staff\
) else (
  if exist Release (
    xcopy /Y /S Release\*.dll %libdir%
    xcopy /Y /S Release\*.dll %AXIS2C_HOME%\modules\staff\
  )
)
xcopy /Y /S config\*.xml %libdir%
xcopy /Y /S config\*.xml %AXIS2C_HOME%\modules\staff\
