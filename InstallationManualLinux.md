# Introduction #

Guide to install WSF Staff under Linux.

# Details #

## 0. Environment variables ##
While compiling, starting Axis2/C or any client/service application, these environment variables must be set:
```
export AXIS2C_HOME=/usr/local/axis2c
export STAFF_HOME=/usr/local/staff
export PATH=$PATH:/usr/local/staff/bin
```

Append these lines into ~/.profile file, and then relogin or execute:
```
. ~/.profile
```

## 1. Compile and install dependencies ##

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
# configure with UTF-8 patched guththila
./configure --prefix=$AXIS2C_HOME --enable-tests=no --enable-amqp=no
make && sudo -E make install
# install axis2/c samples
cd samples
CFLAGS=-I$AXIS2C_HOME/include/axis2-1.6.0 LDFLAGS=-L$AXIS2C_HOME/lib ./configure --prefix=$AXIS2C_HOME
make && sudo -E make install
```


## 2. Getting source code ##

Please refer to http://code.google.com/p/staff/source/checkout to read guide for getting source code from svn repository.
Or download the source code from [Download](http://code.google.com/p/staff/downloads/list) section.


## 3. Compiling source code ##

_Package binutils-dev is required._

Enter these commands to compile and install Staff.

```
make && sudo -E make install
echo -e "$STAFF_HOME/lib\n$AXIS2C_HOME/lib" | sudo tee /etc/ld.so.conf.d/staff.conf
sudo ldconfig
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