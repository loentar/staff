@echo off
set projectname=$(Project.Interfaces.Interface.Namespace.!dot||Project.Interfaces.Interface.Name.!tolower.!dot)

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

#foreach $(Project.Interfaces)
#foreach $(Interface.Classes)
copy /Y \
#ifneq($($bFromInterface),)
$($SRCDIR)\
#ifneq($(Class.Namespace.!dot),)
$(Class.Namespace.!dot).\
#ifeqend
$(Interface.Name).\
#else
$(Interface.FileName) %componentdir%\\
$(Class.NsName.!dot).\
#ifeqend
#ifeq($(Project.Interfaces.$Count),0)
xsd\
#else
wsdl\
#ifeqend
#ifneq($($bFromInterface),)
 \$(INSTALLDIR)\$(COMDIR)\$(COMPONENT)/\
#ifeqend
 >nul
#end
#end

echo Installation of component %projectname% completed