include ../../../Makefile.env

SUBDIR = client/
OUTDIR=$(SUBDIR)out/
TARGETNAME = foo

# == make ===========================================

make: checkdeploy $(OUTDIR)$(TARGETNAME)

$(OUTDIR)$(TARGETNAME):
	staff_codegen -pwsdl -tinterface foo2.wsdl -o$(SUBDIR)
# patch interface
	sed -i '/virtual void fooMethod/s/void/staff::DataObject/' client/foo2.h
# patch proxy.cpp to return serialized request
	staff_codegen -u -tclient foo2.h -c$(SUBDIR)
	sed -i '/\/\/ synchronous call/,/rdoResult = /d;/rOptions/d;s/return rdoResult.*/rdoRequest.SetOwner(false);\n  return rdoRequest;/' client/foo2Proxy.cpp
	$(MAKE) -C$(SUBDIR)

# == test ===========================================
test: make
	unset STAFF_LOG_LEVEL; LD_LIBRARY_PATH="$(LD_LIBRARY_PATH):$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(TARGETNAME)

checkdeploy:
	@test -d $(DEPLOYDIR) || (echo "Please compile staff first."; false)

# == clean ==========================================

clean:
	$(MAKE) -C$(SUBDIR) clean
	rm -rf $(SUBDIR)foo2.h $(SUBDIR)fooTypes1.h $(SUBDIR)fooTypes2.h
