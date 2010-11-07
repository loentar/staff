@echo off
setlocal

  if "%POSTGRESQL_HOME%" == "" (
    echo POSTGRESQL_HOME is not set
    exit 1
  )

:: setting up pass file
  set "PGPASSDIR=%APPDATA%\postgresql"
  set "PGPASSFILE=%PGPASSDIR%\pgpass.conf"
  set line=localhost:5432:testdas:dastestuser:dastestuserpassword
  
  findstr "%line%" "%PGPASSFILE%"
  if not "%ERRORLEVEL%" == "0" (
    echo %line%>>"%PGPASSFILE%"
  )

endlocal

:: install test user and test database
"%POSTGRESQL_HOME%\bin\psql" -Upostgres <testdas.sql
