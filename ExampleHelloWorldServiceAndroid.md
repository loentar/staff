<h1>Table of contents</h1>


## Dependencies ##

  * Must have Android NDK installed.

> To use android gcc cross compiler outside ndk-build script you must create standalone toolchain (required):
```
/opt/android-ndk-r8c/build/tools/make-standalone-toolchain.sh --toolchain=arm-linux-androideabi-4.6 --install-dir=/opt/android-8-toolchain
```

  * Must have [WSF Staff Android app](https://code.google.com/p/staff/downloads/list?can=2&q=WSFStaffAndroid-*.apk) installed on your device/emulator.

  * Must have latest version of WSF Staff installed on your host, refer the InstallationManual.

  * Must have WSF Staff SDK for android installed.

> Download and unpack [WSF Staff SDK for Android](https://code.google.com/p/staff/downloads/list?q=ws-android-*.7z) into /data/local dir on your host.

## Creating a service ##

Start codegen to produce interface template:
```
staff_codegen -tinterfacegen -dsvc=samples.HelloWorld -osrc
```

Now edit generated `src/HelloWorld.h` and add "Hello" operation like that:

```
  class HelloWorld: public staff::IService
  {
  public:
    virtual std::string Hello(const std::string& str) = 0;
  };
```

Next start codegen to produce project and service implementation template:
```
staff_codegen -tcomponent_all -isrc HelloWorld.h
```

Edit `src/HelloWorldImpl.cpp`. Modify "Hello" operation like that:

```
std::string HelloWorldImpl::Hello(const std::string& str)
{
  return "Hello, " + str;
}
```


## Compile the service ##

Set environment variables to match your installation:
```
# host staff installation
export HOST_STAFF_HOME=/usr/local/staff
# armeabi installation of axis2c and staff
export STAFF_HOME=/data/local/staff
export AXIS2C_HOME=/data/local/axis2c
# path to android g++ compiler
export CXX=/opt/android-8-toolchain/bin/arm-linux-androideabi-g++
```

Now compile your service:
```
make
```

## Installing and starting ##

Connect your device by USB with debug enabled or start emulator.

Deploy component to device/emulator:

```
adb push out/arm-linux-androideabi /data/data/org.wsf.staff.android/files/staff/components/samples.HelloWorld
```

Start WSF Staff Android app on your device and start Axis2/C server (stop and start if it already started).

Push the "Open services list" button, your service must appear now in the list.

Now you can connect to that service as usual.

## Download sources ##

Download source code [here](https://code.google.com/p/staff/downloads/detail?name=samples.hello_android.tar.bz2).