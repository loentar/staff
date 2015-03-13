<h1>Table of Contents</h1>



## Notice ##

You should keep `STAFF_HOME` environment variable unsettled while you using WSF Staff installed from PPA.

You should set `AXIS2C_HOME` to `/usr/lib/axis2` else you won't be able to use any client based on Axis2/C.

To set it up you could add this line into `/etc/environment` file:
```
AXIS2C_HOME="/usr/lib/axis2"
```

You should re-login after you set that variable.

## Removing PPA and packages from your system ##

To remove ppa and packages open terminal and enter:
```
sudo apt-get purge staff-libs libapache2-mod-staff-axis2services
sudo rm /etc/apt/sources.list.d/loentar-*.list
sudo apt-get update
```

## Adding PPA to your system ##

**Notice:** To switch from another ppa, for example from old ppa:loentar/ppa or between stable and trunk versions, you should remove previous ppa and previous packages first as described above.

There is two ppa available: stable and trunk.

To add PPA to your system open terminal and enter:
```
sudo apt-add-repository ppa:loentar/<PPA>
sudo apt-get update
```

Where `<PPA>` is one of:
  * `wsf-staff-stable` - for stable version;
  * `wsf-staff-trunk` - for current developing (trunk) version.

You could get more info about that PPAs:

https://launchpad.net/~loentar/+archive/wsf-staff-stable

https://launchpad.net/~loentar/+archive/wsf-staff-trunk


## Installing WSF Staff ##

These packages are available from PPA:
| **Package name** | **Description** |
|:-----------------|:----------------|
| staff-libs | Main package: it contain modules, services and libraries |
| staff-dev | Development package: it contain staff\_codegen and headers |
| libapache2-mod-staff-axis2services | axis2services module to work with apache2 |

### Installing main package ###

To install main package with Axis2/C server you should start:
```
sudo apt-get install staff-libs libaxis2c-bin
```


### Installing development package ###

To install development package you should start:
```
sudo apt-get install staff-dev
```


### Installing Apache2 module axis2services ###

To install axis2services module package you should start:
```
sudo apt-get install libapache2-mod-staff-axis2services
```

Please note, to start apache2 Web Server you should have some apache2-mpm package installed, for example: apache2-mpm-prefork:
```
sudo apt-get install apache2-mpm-prefork
```


## Testing your installation ##

Before you start Axis2/C or staff\_codegen you should ensure that your STAFF\_HOME environment variable is not set.

```
env | grep -qE 'STAFF_HOME' || echo OK
```

You should see 'OK' message.

Else you should unset this variable and remove it from all environment files (like `/etc/environment`, `~/.bashrc`, `~/.profile`, etc..):
```
unset STAFF_HOME
```

### Testing main package installation ###

1. Open new terminal and start Axis2/C:
```
cd /usr/lib/axis2/bin
./axis2_http_server
```

2. Open this URL in a Web Browser: http://localhost:9090/axis2/services

3. You should see Axis2/C services list: staff.wsdl.Wsdl, StaffService, staff.das.DataAccessService and some others.

### Testing development package installation ###

1. Start codegen:
```
staff_codegen -l
```

You should see codegen's template and parser lists.

2. Check headers:
```
ls -d /usr/include/staff
```

You should see:
```
/usr/include/staff
```

3. Now you can try to create some [client](http://code.google.com/p/staff/wiki/ExampleCreatingCppClientFromWsdl) or [service](http://code.google.com/p/staff/wiki/ExampleCalculatorService).


### Testing Apache2 module axis2services installation ###

1. Apache2 Web Server should be started.

2. Open this URL in a Web Browser: http://localhost/axis2/services

3. You should see Axis2/C services list: staff.wsdl.Wsdl, StaffService, staff.das.DataAccessService and some others.