To install WSF Staff under FreeBSD, please follow InstallationManualLinux with a following exceptions:

  * **par 3.**<br />Enter these commands to compile and install staff.
```
gmake && sudo -E gmake install
sudo chmod -R go-w /usr/local/staff/lib
sudo chmod 777 $AXIS2C_HOME/logs
sudo ldconfig $STAFF_HOME/lib $AXIS2C_HOME/lib
```