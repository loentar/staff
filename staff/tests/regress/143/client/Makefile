
OUTDIR = test_client

make:
	mkdir -p $(OUTDIR) && cd $(OUTDIR) && staff_codegen -pwsdl -tclient_all http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl

test: make
	$(MAKE) -C$(OUTDIR)

clean:
	rm -Rf $(OUTDIR)