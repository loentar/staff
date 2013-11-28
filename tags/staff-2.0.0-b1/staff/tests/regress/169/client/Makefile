
OUTDIR = test_client

make:
	staff_codegen -pwsdl -tclient_all test.wsdl -o$(OUTDIR)
	sed -i '/Invoke Your service here:/{s/$$/\n    ptest->test_op(_plus_L1);/}' $(OUTDIR)/src/main.cpp

test: make
	$(MAKE) -C$(OUTDIR)

clean:
	rm -Rf $(OUTDIR)