## 0. Versions ##
Two versions of binary distribution are available: built in MSVC and GCC.

## 1. Set environment variables ##
While starting Axis2/C server or any client/service application, these environment variables, must be set:
```
AXIS2C_HOME=c:\ws\axis2c
STAFF_HOME=c:\ws\staff
PATH=%AXIS2C_HOME%\lib;%STAFF_HOME%\lib;%STAFF_HOME%\bin
```

You can set these variables in system properties. After setting these variables, You must relogin or reboot Windows.

## 2. Install ##
Unpack archive file `ws-<ARCH>-vs2005-staff-<STAFF_VERSION>.7z` or `ws-win32-gccXXX-staff-<STAFF_VERSION>.7z` to c:\

Where `<ARCH>` is a package architecture: win32 or win64.

You should see following directories:
```
c:\ws\axis2c
c:\ws\staff
```

## 3. Start Axis2/c ##
Open command prompt and enter:
```
cd %AXIS2C_HOME%\bin
axis2_http_server
```

Axis2/C must start.

> |_If You get error "Cannot execute file" or similar, possibly You need to install vs2005 vcredist\_x86.exe package (occurs only under winxp and prior)._<p>If Axis2/C server finishes immediately after starting, please see c:\ws\axis2c\logs\axis2.log to diagnose the problem.<br />The most common problem is in the wrong setting environment variables.</li></ul></tbody></table>

<h2>4. Services list</h2>
Open web browser and enter this address:<br>
<br>
<a href='http://localhost:9090/axis2/services'>http://localhost:9090/axis2/services</a>

Services list must shown.<br>
<br>
<h2>5. Start sample "calc"</h2>
Open command prompt and enter:<br>
<pre><code>%STAFF_HOME%\samples\bin\calc_client<br>
</code></pre>

Service execution results must shown.