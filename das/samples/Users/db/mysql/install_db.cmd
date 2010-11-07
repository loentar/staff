@echo off

if "%MYSQL_HOME%" == "" (
  echo MYSQL_HOME is not set
  exit 1
)

echo User: root

:: install user and test database
"%MYSQL_HOME%\bin\mysql" -p -u root mysql < testdas.sql
