include ../../../../Makefile.env

INTERFACES := Test135.h
TARGET = staff_test_regress_$(notdir $(patsubst %/,%,$(dir $(CURDIR))))

CXXFLAGS += -c -I$(DEPLOYDIR)$(INCDIR)
LDFLAGS += -lstaffclient -lrise -L$(DEPLOYDIR)$(LIBDIR)

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
	$(STAFF_HOME)/bin/staff_codegen -c$(SRCDIR) $(INTERFACES)

checkdeploy:
	@test -d $(DEPLOYDIR) || (echo "Please compile staff first."; false)
