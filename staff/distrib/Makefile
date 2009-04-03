include ../Makefile.env

make:;
clean:;

distrib:
	mkdir -p $(DEPLOYROOTDIR)etc/ld.so.conf.d/ $(DEPLOYROOTDIR)etc/profile.d/
	echo "/usr/local/staff/lib" >$(DEPLOYROOTDIR)etc/ld.so.conf.d/staff.conf
	echo "/usr/local/axis2c/lib" >$(DEPLOYROOTDIR)etc/ld.so.conf.d/axis2c.conf
	echo -e "# axis2c env\n\nif [ -z \"\$$AXIS2C_HOME\" ]\nthen\n  export AXIS2C_HOME=/usr/local/axis2c\nfi" >$(DEPLOYROOTDIR)etc/profile.d/axis2c.sh
	echo -e "# staff env\n\nif [ -z \"\$$STAFF_HOME\" ]\nthen\n  export STAFF_HOME=/usr/local/staff\n  export PATH=\$$PATH:\$$STAFF_HOME/bin\nfi" >$(DEPLOYROOTDIR)etc/profile.d/staff.sh
	chmod +x $(DEPLOYROOTDIR)etc/profile.d/axis2c.sh $(DEPLOYROOTDIR)etc/profile.d/staff.sh