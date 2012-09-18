
OUTDIR = test_client

make:
	staff_codegen -pwsdl -tclient_all test.wsdl -o$(OUTDIR)

test: make
	$(MAKE) -C$(OUTDIR)

clean:
	rm -Rf $(OUTDIR)