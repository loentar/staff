include ../../../Makefile.env

SUBDIR = client/
OUTDIR=$(SUBDIR)out/
TARGETNAME = foo

# == make ===========================================

make: checkdeploy $(OUTDIR)$(TARGETNAME)

$(OUTDIR)$(TARGETNAME):
	staff_codegen -pwsdl -tinterface foo.wsdl -o$(SUBDIR)
# patch interface
	sed -i '/virtual ::fooResponseType fooMethod/s/::fooResponseType/staff::DataObject/' client/foo.h
# patch proxy.cpp to return serialized request
	staff_codegen -u -tclient foo.h -c$(SUBDIR)
	sed -i '/\/\/ synchronous call/,/rdoResult = /d;s/return rdoResult.*/rdoRequest.SetOwner(false);\n  return rdoRequest;/' client/fooProxy.cpp
	$(MAKE) -C$(SUBDIR)

# == test ===========================================
test: make
	unset STAFF_LOG_LEVEL; LD_LIBRARY_PATH="$(LD_LIBRARY_PATH):$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(TARGETNAME)

checkdeploy:
	@test -d $(DEPLOYDIR) || (echo "Please compile staff first."; false)

# == clean ==========================================

clean:
	$(MAKE) -C$(SUBDIR) clean
	rm -rf $(SUBDIR)foo.h $(SUBDIR)fooXsd.h $(SUBDIR)fooCommon.h
