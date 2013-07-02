
SUBDIR = test_client

make:
	cd $(SUBDIR) && staff_codegen -tclient_all -isrc foo.h
	$(MAKE) -C$(SUBDIR)

test: make

clean:
	$(MAKE) -C$(SUBDIR) clean
	rm -rf $(SUBDIR)/src/main.cpp
	rm -rf $(SUBDIR)/codegen.cmd
	rm -rf $(SUBDIR)/ns_staff_foo.pro
	rm -rf $(SUBDIR)/ns_staff_foo.vcproj
	rm -rf $(SUBDIR)/Makefile
