# Introduction #

Guide to install WSF Staff under Windows.

# Details #

## 0. Environment variables ##
While compiling, starting Axis2/C or any client/service application, these environment variables, must set:
```
AXIS2C_HOME=c:\ws\axis2c
STAFF_HOME=c:\ws\staff
PATH=%PATH%;%AXIS2C_HOME%/bin;%AXIS2C_HOME%/lib;%STAFF_HOME%/bin;%STAFF_HOME%/lib
```

While compiling DAS providers MySQL and PostgreSQL this variables must set to DBMS home:
```
MYSQL_HOME=c:\Program Files\MySQL\MySQL Server 5.1
POSTGRESQL_HOME=c:\Program Files\PostgreSQL\8.3
```

If You do not needed PostgreSQL or MySQL provider, do not set corresponding variable.

You can set these variables in system properties.
After setting these variables, You must re-login or reboot windows.


## 1. Compile and install dependencies ##

> |_Please note: Axis2/C team delivers only 32-bit version of Axis2/C._<br />If You need 64-bit version, please obtain it from our [Download section](http://code.google.com/p/staff/downloads/list?q=ws-win64).|
|:------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

You need axis2/c-1.5.0 or greater to be installed. Please, refer to http://ws.apache.org/axis2/c/download.cgi.

Unpack directory contents `axis2c-bin-1.5.0-win32` into c:\ws\axis2c

Make sure that Axis2/С starts:
```
c:\ws\axis2c\bin\axis2_http_server.exe
```

It must be console window with text:
```
Started Simple Axis2 HTTP Server ...
```
If You encounter some errors, please refer to Axis2/C site or get `ws-win<ARCH>-bin*.7z` from our download section.

Make sure, that Axis2/C is working properly, take a look at:
http://localhost:9090/axis2/services/
It must be Axis2/С service list.


## 2. Getting source code ##

Please refer to http://code.google.com/p/staff/source/checkout to read guide for getting source code from svn repository.
Or checkout the [Download](http://code.google.com/p/staff/downloads/list) section.


## 3. Compiling and installing ##

The compilation is available from the command line and from the IDE. You must choose variant do you like, 3.1 or 3.2.

## 3.1 Compiling and installing from command line ##

_Note: If You using VS2008 or greater, solution upgrading must made automatically._

Open command prompt from staff directory and start:
```
  build <TARGET>
```

where `<TARGET>` is version to build: `Debug`, `Release`, `"Release|Win32"`, `"Debug|Win32"`, `"Release|x64"`, `"Debug|x64"` (case sensitive, including quotation marks).

When build successes, start:
```
  install <ARCH>
```

Where `<ARCH>` is an architecture configuration: Win32 or x64.

### 3.2 Compiling from Visual Studio ###

_Note: If You using VS2008 or greater, after opening this solution "Visual Studio Conversion Wizard" will be appear. Process solution upgrade to continue._

To compile from the IDE Visual Studio:

Open staff.sln from the directory staff, then build the solution (menu Build->Build Solution).

To install open command prompt from staff directory and start:
```
  install <ARCH>
```

Where `<ARCH>` is an architecture configuration: Win32 or x64.

## 4. Starting ##

### 4.1 Starting in normal mode ###

To start Axis2/C in normal mode:
```
cd %AXIS2C_HOME%\bin
axis2_http_server
```

### 4.2 Starting in debug mode ###

To start Axis2/C in debug mode:
```
cd %AXIS2C_HOME%\bin
set STAFF_LOG_LEVEL=DEBUG3
set STAFF_EXCEPTION_STACKTRACING=1
axis2_http_server
```


## 5. Testing ##

### 5.1 Basic test ###

Open the web-browser and enter:

http://localhost:9090/axis2/services

Sample services "samples.calc.CalcService", "samples.calc.SubService" must running.


### 5.2 Service-Client interaction test ###

Open command prompt and start calc client:
```
%STAFF_HOME%\samples\bin\calc_client
```

Screen output:
```
INFO main.cpp[49]: main: 1 + 2 = 3
INFO main.cpp[51]: main: 3 - 1 = 2
INFO main.cpp[53]: main: Saved service mem: 0
INFO main.cpp[58]: main: Setting mem: 46
INFO main.cpp[60]: main: 46
INFO main.cpp[61]: main: Service mem: 46
```