@echo off

if "%STAFF_HOME%" == "" (
  echo STAFF_HOME is not set
  exit 1
)


:: install test database
xcopy /y /s testdas.db %STAFF_HOME%\db\
