
SUBDIR = test_client

make:
	mkdir -p $(SUBDIR)
	staff_codegen -tclient_all -pwsdl ld_types.wsdl -o$(SUBDIR)
	$(MAKE) -C$(SUBDIR)

test: make

clean:
	rm -rf $(SUBDIR)