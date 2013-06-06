SUBDIR = test_client

.PHONY: test

# == make ===========================================
make:
	mkdir -p $(SUBDIR)
	cp -f staff.tests.Test147.wsdl $(SUBDIR)/
	cd $(SUBDIR) && staff_codegen -tclient_all -pwsdl staff.tests.Test147.wsdl
	$(MAKE) -C $(SUBDIR)

# == clean ==========================================
clean:
	rm -rf $(SUBDIR)

# == test ===========================================
test: make
