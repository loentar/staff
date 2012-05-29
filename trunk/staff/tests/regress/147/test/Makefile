include ../../../../Makefile.env

TARGET = staff_test_regress_$(notdir $(patsubst %/,%,$(dir $(CURDIR))))

CXXFLAGS += -c -I$(DEPLOYDIR)$(INCDIR)
LDFLAGS += -lstaffclient -L$(DEPLOYDIR)$(LIBDIR)

.PHONY: test
.NOTPARALLEL: $(CODEGEN_FILES)

# == make ===========================================
make:;

# == clean ==========================================
clean:
	rm -f $(OUTDIR)$(TARGET) $(OBJECTS) $(CODEGEN_FILES)
	test ! -d $(OBJDIR) || rmdir -p $(OBJDIR)
	test ! -d $(OUTDIR) || rmdir -p $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@

# == test ===========================================
test:
	mkdir -p test_client; cp -f staff.tests.Test147.wsdl test_client/; cd test_client && staff_codegen -tclient_all -pwsdl staff.tests.Test147.wsdl
	$(MAKE) -C test_client

checkdeploy:
	@test -d $(DEPLOYDIR) || (echo "Please compile staff first."; false)
