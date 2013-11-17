
SUBDIR = test_client

make:
	cd $(SUBDIR) && staff_codegen -tclient_all -isrc floatPrec.h -e -u -dnoimpl
	sed -i 's/$$(STAFF_CODEGEN)/& -e/g' $(SUBDIR)/Makefile
	$(MAKE) -C$(SUBDIR)
	$(MAKE) -C$(SUBDIR) run

test: make

clean:
	test \! -f $(SUBDIR)/Makefile || $(MAKE) -C$(SUBDIR) clean
	rm -f $(SUBDIR)/Makefile $(SUBDIR)/*.pro $(SUBDIR)/*.vcproj $(SUBDIR)/*.cmd
