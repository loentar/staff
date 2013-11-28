@echo off
set projectname=staff.samples.das.services

set target=%1%
if "%target%" == "" (
  if exist Release (
    set target=Release
  ) else (
    if exist Debug (
      set target=Debug
    ) else (
      echo Can't detect target directory.
      echo Please start as "install <TARGET>", where <TARGET> is your target directory name.
      pause
      exit 1
    )
  )
)

echo Using %target% version.

set componentdir=%STAFF_HOME%\components\%projectname%

if not EXIST %componentdir% mkdir %componentdir%

copy /Y %target%\*.dll %componentdir%\ >nul

copy /Y \
src/\
staff.samples.das.services.\
Accounts.\
wsdl\
 $(INSTALLDIR)$(COMDIR)$(COMPONENT)/\
 >nul

echo Installation of component %projectname% completed