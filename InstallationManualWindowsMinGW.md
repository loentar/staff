# Introduction #

Guide to compile and install WSF Staff under Windows using GCC under MinGW and MSys.

# Details #

## 1. Install dependencies ##

### 1.1 MinGW ###

Download MinGW installer from [Sourceforge](http://sourceforge.net/projects/mingw/files/latest/download?source=files) and start it.

At components page select "C++ Compiler" and "MSYS Basic System" to be installed.

If you need to compile Staff against Qt version of MinGW you can select "MSYS Basic System" only.

### 1.2 Axis2/C ###

It is not possible to compile Axis2/C under MinGW at current moment. So we will use Axis2/C compiled with VS2005 compiler with special options (with undecorated names). This is compatible with gcc.

To compile Axis2/C from source code follow the 1.2.1.

**Or** to use prebuilt version of Axis2/C follow the 1.2.2.

#### 1.2.1 Building Axis2/C from source code ####

You can compile Axis2/C that is compatible with GCC.

Download Axis2/C source code [here](http://www.sai.msu.su/apache//ws/axis2/c/1_6_0/axis2c-src-1.6.0.zip), unpack it and apply the patch located [here](http://code.google.com/p/staff/downloads/detail?name=axis2c-src-1.6.0-mingw.patch).

If you have no patch utility install it using MinGW installer.

To apply the patch start bash and enter:
```
cd axis2c-src-1.6.0
patch -d. -p1 <../axis2c-src-1.6.0-mingw.patch 
```

Next start Visual Studio command prompt, cd to the `axis2c-src-1.6.0\build\win32` directory and start:

```
nmake install
```

Axis2/C deploy will be produced in `axis2c-src-1.6.0\build\deploy` directory.
Copy it's contents into `c:\ws\axis2c`.

#### 1.2.2 Using prebuilt version of Axis2/C ####

Or you can use precompiled version from [download section](http://code.google.com/p/staff/downloads/list?q=gcc4).

Unpack `axis2c` directory from downloaded tarball into `c:\ws\`.

## 2. Environment variables ##
While compiling, starting Axis2/C or any client/service application, these environment variables, must set:
```
AXIS2C_HOME=c:\ws\axis2c
STAFF_HOME=c:\ws\staff
PATH=c:\MinGW\msys\1.0\bin;PATH_TO_MINGW_BIN;%AXIS2C_HOME%\bin;%AXIS2C_HOME%\lib;%STAFF_HOME%\bin;%STAFF_HOME%\lib
```

Where `PATH_TO_MINGW_BIN` is:
  * `c:\MinGW\bin` if you using GCC from MinGW;
  * `c:\QtSDK\mingw\bin` if you using GCC from QtSDK.

> | _Please note: you should have path to msys's bin before any %SystemRoot% else your build will fail._ |
|:-----------------------------------------------------------------------------------------------------|

You can set these variables in system properties in "Environment Variables" => "System variables".
After these variables is set, You must re-login or reboot windows.


## 3. Getting source code ##

Please refer to http://code.google.com/p/staff/source/checkout to read guide for getting source code from svn repository.
Or checkout the [Download](http://code.google.com/p/staff/downloads/list) section.


## 4. Compiling and installing ##

Enter source code directory and start bash.exe there.
To compile Staff and samples start:
```
# compile and install staff
make && make install
#compile and install samples
cd staff/samples
make && make install
```


## 5. Starting ##

### 5.1 Starting in normal mode ###

To start Axis2/C in normal mode:
```
cd %AXIS2C_HOME%\bin
axis2_http_server
```

### 5.2 Starting in debug mode ###

To start Axis2/C in debug mode:
```
cd %AXIS2C_HOME%\bin
set STAFF_LOG_LEVEL=DEBUG3
set STAFF_EXCEPTION_STACKTRACING=1
axis2_http_server
```


## 6. Testing ##

### 6.1 Basic test ###

Open the web-browser and enter:

http://localhost:9090/axis2/services

Sample services "samples.calc.CalcService", "samples.calc.SubService" must running.


### 6.2 Service-Client interaction test ###

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