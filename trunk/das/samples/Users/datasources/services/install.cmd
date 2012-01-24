@echo off
rem start this file to install datasources with services provider

if "%STAFF_HOME%" == "" (
  echo Error: STAFF_HOME is not set
  goto errexit
)

set outdir=%STAFF_HOME%\components\staff.das\datasources\staff.das.samples\

if not exist %outdir% mkdir %outdir%

xcopy /Y /S datasources\*.* %outdir%

set wsdldir=%STAFF_HOME%\components\staff.das.samples
if not exist %wsdldir% mkdir %wsdldir%

staff_codegen -twsdl -pdasdatasource -idatasources Groups.datasources Users.datasources -o%wsdldir%

:errexit