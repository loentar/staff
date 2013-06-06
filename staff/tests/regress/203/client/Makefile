
OUTDIR = test_client

make:
	staff_codegen -pwsdl -tinterface test.wsdl -o$(OUTDIR)

test: make
	grep -q "// \*interface\.targetNamespace: http://foons" $(OUTDIR)/test.h

clean:
	rm -Rf $(OUTDIR)