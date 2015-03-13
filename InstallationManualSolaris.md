# Introduction #

Guide to install WSF Staff under SunOS/Solaris.

| Notice: All these instructions where tested under SunOS 5.10 32 bit with a SPARC architecture, if you find any problems installing it please let us know |
|:---------------------------------------------------------------------------------------------------------------------------------------------------------|

# Details #

## 0. Environment variables ##
While compiling, starting Axis2/C or any client/service application, these environment variables must be set:
```
export AXIS2C_HOME=/usr/local/axis2c
export STAFF_HOME=/usr/local/staff
export PATH=$PATH:$STAFF_HOME/bin
```

Append these lines into ~/.profile file, and then relogin or execute:
```
. ~/.profile
```

## 1. Installing the dependencies ##

To install WSF Staff under Solaris you need bash, binutils, Autotools and pkg-config.
Autotools is a set of utilities needed to build source code that is checked out from repository. Pkg-config provides interface to query installed libraries.

Getting this packages varies depending if you're using a licensed distribution or not.
Free versions of the packages are available here: http://sunfreeware.com/

_Dependencies_

| _**Mandatory**_ |
|:----------------|
| bash |
| gcc  |
| binutils  |
| pkg-config |
| _**Optional**_ |
| sudo |
| aclocal |
| autoheader |
| autoconf |
| automake |
| libtool |

## 2. Installing Axis2/C ##

There are 2 different ways to install Axis2/C:
  * From unofficial repository (recommended)
  * From official source

Please notice that installing from unofficial repository is recommended, because it has many improvements over Axis2/C 1.6 branch.

### 2.1. Installing Axis2/C from unofficial repository (recommended) ###

Please refer to: https://code.google.com/p/axis2c-unofficial/wiki/InstallationManualSolaris

### 2.2. Installing Axis2/C from official source ###
You need Axis2/C version 1.6.0 to be installed. Please, refer to http://axis.apache.org/axis2/c/core/docs/installationguide.html.

To download and install axis2/c you can use commands as follows:
```
mkdir -p ~/devel/axis2c
cd ~/devel/axis2c
wget http://www.sai.msu.su/apache/ws/axis2/c/1_6_0/axis2c-src-1.6.0.tar.gz
tar zxf axis2c-src-1.6.0.tar.gz
cd axis2c-src-1.6.0
# download and apply guththila UTF-8 patch
wget https://issues.apache.org/jira/secure/attachment/12407627/utf8-patch.txt
patch -d. -p0 <utf8-patch.txt
# download and apply Solaris patch
wget https://issues.apache.org/jira/secure/attachment/12582961/axis2c_solaris.patch
patch -d. -p0 <axis2c_solaris.patch
# configure with UTF-8 patched guththila
./configure --prefix=$AXIS2C_HOME --enable-tests=no --enable-amqp=no
make && sudo -E make install
# install axis2/c samples
cd samples
CFLAGS=-I$AXIS2C_HOME/include/axis2-1.6.0 LDFLAGS=-L$AXIS2C_HOME/lib ./configure --prefix=$AXIS2C_HOME
make && sudo -E make install
```

## 3. Getting source code ##

Please refer to http://code.google.com/p/staff/source/checkout to read guide for getting source code from svn repository.
Or download the source code from [Download](http://code.google.com/p/staff/downloads/list) section.


## 4. Compiling source code ##

Enter these commands to compile and install Staff.

```
make && sudo -E make install
crle -u -l $STAFF_HOME:$AXIS2C_HOME
sudo chmod 777 $AXIS2C_HOME/logs
```

To compile and install samples start:
```
cd staff/samples
make && sudo -E make install
```

## 4. Starting ##

### 4.1 Starting in normal mode ###

To start axis2/c in normal mode, please enter:
```
cd $AXIS2C_HOME/bin
./axis2_http_server
```

### 4.2 Starting in debug mode ###

To start axis2/c in debug mode, please enter:
```
cd $AXIS2C_HOME/bin
STAFF_LOG_LEVEL=DEBUG3 STAFF_EXCEPTION_STACKTRACING=1 ./axis2_http_server
```


## 5. Testing ##

### 5.1 Basic test ###

Open Your favorite web-browser and enter:

http://localhost:9090/axis2/services

Sample services "samples.calc.CalcService", "samples.calc.SubService" must running.


### 5.2 Service-Client interaction test ###

Open console and start calc client:
```
${STAFF_HOME}/samples/bin/calc_client
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