include ../../../Makefile.env

SUBDIR = client/
OUTDIR=$(SUBDIR)out/
TARGETNAME = foo

# == make ===========================================

make: checkdeploy $(OUTDIR)$(TARGETNAME)

$(OUTDIR)$(TARGETNAME):
	staff_codegen -pwsdl -tinterface foo.wsdl -o$(SUBDIR)
	staff_codegen -u -tclient foo.h -i$(SUBDIR) -o$(SUBDIR)
	$(MAKE) -C$(SUBDIR)

# == test ===========================================
test: make
	unset STAFF_LOG_LEVEL; LD_LIBRARY_PATH="$(LD_LIBRARY_PATH):$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(TARGETNAME)

checkdeploy:
	@test -d $(DEPLOYDIR) || (echo "Please compile staff first."; false)

# == clean ==========================================

clean:
	$(MAKE) -C$(SUBDIR) clean
	rm -rf $(SUBDIR)foo.h $(SUBDIR)fooXsd.h $(SUBDIR)fooCommon.h $(SUBDIR)fooProxy.h $(SUBDIR)fooProxy.cpp $(SUBDIR)fooXsdProxy.h  $(SUBDIR)fooXsdProxy.cpp $(SUBDIR)fooCommonProxy.h $(SUBDIR)fooCommonProxy.cpp
