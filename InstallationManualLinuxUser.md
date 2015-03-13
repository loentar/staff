## Description ##

This is a short guide on how to install WSF Staff without having root access.

> _If you have root access and want to install it into system directories, just replace `AXIS2C_HOME` and `STAFF_HOME` with the preferred values, for example: `AXIS2C_HOME=/usr/local/axis2c` and `STAFF_HOME=/usr/local/staff`. When starting `make install` start it like this: `sudo -E make install`._

## Build tools ##

| **You must have build tools installed.** <br>If you don't have permissions to install it, ask your system administrator or try to install it into writable location and set environment/patch tools to get it working from that location. </tbody></table>

<pre><code>sudo apt-get install automake autoconf libtool pkg-config binutils-dev g++ gcc subversion mercurial<br>
</code></pre>

<h2>Prepare environment ##

Set `AXIS2C_HOME` and `STAFF_HOME` to any writable location:

```
export AXIS2C_HOME=~/opt/axis2c
export STAFF_HOME=~/opt/staff
export LD_LIBRARY_PATH=$AXIS2C_HOME/lib:$STAFF_HOME/lib:$LD_LIBRARY_PATH

mkdir ~/devel
```

## Install Axis2/C ##

```
hg clone https://code.google.com/p/axis2c-unofficial/
cd axis2c-unofficial/
./autogen.sh
./configure --prefix=$AXIS2C_HOME
make -j8
make install
mkdir $AXIS2C_HOME/logs
```

## Install WSF Staff ##

```
cd ..
svn co http://staff.googlecode.com/svn/trunk/ staff
cd staff
make -j8
make install
```

## Install WSF Staff examples (optional) ##

```
cd staff/samples
make -j8
make install
```

## Startup script for Axis2/C ##

This script is needed to setup proper environment and start Axis2/C with Staff.

```
cat <<EOF >$AXIS2C_HOME/bin/start_axis2c
export AXIS2C_HOME=~/opt/axis2c
export STAFF_HOME=~/opt/staff
export LD_LIBRARY_PATH=$AXIS2C_HOME/lib:$STAFF_HOME/lib:$LD_LIBRARY_PATH
cd $AXIS2C_HOME/bin
./axis2_http_server \$@
EOF
chmod +x $AXIS2C_HOME/bin/start_axis2c
```

If you want to install it under root replace the first line as follows:
```
cat <<EOF | sudo tee $AXIS2C_HOME/bin/start_axis2c
```


## Start Axis2/C ##

Now you have Axis2/C and Staff installed in `~/opt/`. To start Axis2/C with Staff:

```
~/opt/axis2c/bin/start_axis2c
```