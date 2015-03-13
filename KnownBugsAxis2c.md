<h1>Table of Contents</h1>


# Introduction #

Staff is based on Axis2/C framework.

As last Axis2/C release 1.6.0 is a bit outdated, there are many known issues on it that influence Staff, but they will probably be fixed in the new Axis2/C release.

This list doesn't have every single Axis2/C issue, just some of the issues that staff users have runned into. If you find another issue related to Axis2/C please let us know at by creating new issue with `Component-Axis2C` label set. Feel free to attach your patch (if any) or some other helpful information.

## Error while serializing/deserializing xml with special chars (latin, chinese chars, etc) ##

### Problem ###

While doing operations with special characters (sending requests, receiving responses, etc), staff rises an error message similar to this:

```
Exception: src/DataObject.cpp[3910] FromString: No Root Element in the pAxiomDoc
```

### Explanation ###

It is an Axis2/C issue that rises when using guhthilla xml parser.
The Axis2/C guhthilla parser originally didn't support UTF-8 encoding.
For more details see: https://issues.apache.org/jira/browse/AXIS2C-1265

### Solution ###

Axis2/C must be built with the patch attached to the jira issue:

```
# go to axis2/c source code directory
cd ~/devel/axis2c
# cleanup build
make distclean
# get patch
wget https://issues.apache.org/jira/secure/attachment/12407627/utf8-patch.txt
# apply the patch
patch -d. -p0 <utf8-patch.txt
```

Then you must recompile Axis2/C and staff as in InstallationManualLinux (don't forget to clean up staff's source code by `make distclean`).

After installation you should be able to work with UTF-8 chars.

## Double request is sent in HTTP authenticated clients ##

### Problem ###

When HTTP Basic authentication is set, every request sent by the client is resent twice.

### Explanation ###

There is a missing brace in http authentication code in Axis2/C
For more details see: https://issues.apache.org/jira/browse/AXIS2C-1244

### Solution ###

Axis2/C must be built with the patch attached to the jira issue:

```
# go to axis2/c source code directory
cd ~/devel/axis2c
# cleanup build
make distclean
# get patch
wget https://issues.apache.org/jira/secure/attachment/12408133/axis2c-1244.txt
# apply the patch
patch -d. -p0 <axis2c-1244.txt
```

Then you must recompile Axis2/C and staff as in InstallationManualLinux (don't forget to clean up staff's source code by `make distclean`).

After installation there should be no duplicate requests.

## Segfault, while using 2 clients of the same service with default and non-default instanceid. ##

### Problem ###

There seems to be crashes while using 2 clients of the same service from different threads.

### Explanation ###

It is an Axis2/C issue that rises when using libxml xml parser.
There seems to be crashes while using libxml from multiple threads.

### Solution ###

It's a libxml bug. Please recompile axis2/c with guththila without libxml to avoid this bug.

```
# go to axis2/c source code directory
./configure --enable-libxml=no --enable-guhthilla=yes
```

Then you must recompile Axis2/C and staff as in InstallationManualLinux (don't forget to clean up staff's source code by `make distclean`).

## Client crashes with asynchronous client calls and/or under high load ##

### Problem ###

While using multiple staff clients/ reusing callbacks, the client crashes.

### Explanation ###

There seem to be 2 different issues:
  * There is a problem when releasing the callback in Axis2/C.
  * There are multiple file uses that are not being validated and fail under load conditions (regardless of whether the client is synchronous or not)

### Solution ###

This issue hasn't been fixed yet, there are many issues in Axis2/C's jira regarding this issue. A partial fix has been proposed in https://issues.apache.org/jira/browse/AXIS2C-1584 which solves the problem regarding callbacks, but not the failures for multiple clients...

If someone finds a bug fix for this, please let us know.

## HTTP\_METHOD DELETE and REST does not work ##

### Problem ###

While invoking a REST service with HTTP DELETE method request fails with HTTP status code 500.

### Explanation ###

Original issue description and patch for Axis/C-1.7.0 is here https://issues.apache.org/jira/browse/AXIS2C-1418.

### Solution ###

To fix Axis2/C-1.6.0 you can use patch located in [download section](http://code.google.com/p/staff/downloads/detail?name=http_transport_utils.c.REST_DELETE.patch).
This patch is based on AXIS2C-1418 but adopted to use with Axis2/C-1.6.0.


## Source back compatibility of Axis2/C-1.7.0 with 1.6.0 ##
|Notice: This issue has been solved for Axis2/C trunk currently, the issue only arises for Axis revisions prior to 1406900  |
|:--------------------------------------------------------------------------------------------------------------------------|

### Problem ###
Staff does not compile on clean Axis2/C-1.7.0 from trunk.

### Explanation ###

Axis2/C has OM (Object Model) API changes that prevents Staff from compiling on it. Some necessary functions like `axiom_element_remove_attribute` were commented out in AxiOM header file.

See issue description on Axis2/C Jira tracker: https://issues.apache.org/jira/browse/AXIS2C-1553

### Solution ###

To fix Axis2/C-1.7.0 you must apply the patch located on link above.

To make build of Axis2/C-1.7.0 and staff you can use commands sequence as follows:

```
export AXIS2C_HOME=/usr/local/axis2c
export STAFF_HOME=/usr/local/staff
rm -Rf $AXIS2C_HOME $STAFF_HOME
mkdir ~/install
cd ~/install
svn co http://svn.apache.org/repos/asf/axis/axis2/c/core/trunk axis2c-trunk -r 1406900
cd axis2c-trunk
sed -i '/SUBDIRS/s/ test//' neethi/Makefile.am # else it does not compile
wget -qO- http://staff.googlecode.com/files/axis2c-1.7.0-om.patch | patch -p1 -d. # apply patch
autoreconf -vifs
./configure --prefix=$AXIS2C_HOME --enable-tests=no --enable-amqp=no
make -j8 && sudo -E make install
cd ..
svn checkout http://staff.googlecode.com/svn/trunk/ staff-trunk
cd staff-trunk
make -j6 && sudo -E make install
echo -e "$STAFF_HOME/lib\n$AXIS2C_HOME/lib" | sudo tee /etc/ld.so.conf.d/staff.conf
sudo ldconfig
cd staff/samples
make -j6 && sudo -E make install
```