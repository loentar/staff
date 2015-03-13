## 1. Install Android SDK ##

Install Android SDK as described here: http://developer.android.com/sdk/installing/index.html

Or just obtain adb executable from somewhere.

## 2. Download and unpack ##

Download archive from https://code.google.com/p/staff/downloads/list?q=ws-android-armeabi

Unpack and remove files not needed on target device (headers and docs) to save some space:
```
7z x ws-android-armeabi*.7z
rm -rf axis2c/docs axis2c/include staff/include
```

## 3. Installing on device ##

Unlock your device and enable USB debugging in "Settings" => "Developer options" => "USB debugging".

Plug-in your device to PC using USB cable.

Start terminal emulator and enter:

```
sudo <PATH_TO_ADB>/adb start-server
<PATH_TO_ADB>/adb devices
```

You should see devices list like that:
```
List of devices attached 
XXXXXXXXXXXXXXX	device

```

where XXXXXXXXXXXXXXX - is serial number of your device.


To install on device start:
```
<PATH_TO_ADB>/adb push axis2c /data/local/axis2c/
<PATH_TO_ADB>/adb push staff /data/local/staff/
<PATH_TO_ADB>/adb shell mkdir /data/local/axis2c/logs
# if you are under windows
<PATH_TO_ADB>/adb shell chmod 777 /data/local/axis2c/bin/axis2_http_server
```

## 4. Start Axis2/C on Android device ##

Open terminal emulator and enter:

```
<PATH_TO_ADB>/adb shell
# you are on your Android device now
export LD_LIBRARY_PATH=../lib:../../staff/lib
export AXIS2C_HOME=/data/local/axis2c
export STAFF_HOME=/data/local/staff
cd $AXIS2C_HOME/bin
./axis2_http_server
```

You must see message like that:
```
StaffService started (version 1.99.1-r728)
Started Simple Axis2 HTTP Server ...
```


## 5. Listing services ##

Open browser on your device and enter URL: http://localhost:9090/axis2/services

you must see services list.


Also you can access device list from other device/PC entering the same URL, replacing 'localhost' to target device IP.