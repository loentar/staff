include ../Makefile.env

make:;
clean:;
install:;
uninstall:;

distrib:
	mkdir -p $(DEPLOYROOTDIR)etc/ld.so.conf.d/ $(DEPLOYROOTDIR)etc/profile.d/
	echo "$(STAFF_HOME)/lib" >$(DEPLOYROOTDIR)etc/ld.so.conf.d/staff.conf
	echo -e "# staff env\n\nif [ -z \"\$$STAFF_HOME\" ]\nthen\n  export STAFF_HOME=$(STAFF_HOME)\n  export PATH=\$$PATH:\$$STAFF_HOME/bin\nfi" >$(DEPLOYROOTDIR)etc/profile.d/staff.sh
	chmod +x $(DEPLOYROOTDIR)etc/profile.d/axis2c.sh $(DEPLOYROOTDIR)etc/profile.d/staff.sh
