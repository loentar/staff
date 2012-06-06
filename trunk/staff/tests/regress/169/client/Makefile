
OUTDIR = test_client

make:
	staff_codegen -pwsdl -tclient_all test.wsdl -o$(OUTDIR)
	sed -i '/Invoke Your service here:/{s/$$/\nconst ::response\& response = ptest->test_op(_0x2dL1);/}' $(OUTDIR)/src/main.cpp

test: make
	$(MAKE) -C$(OUTDIR)

clean:
	rm -Rf $(OUTDIR)