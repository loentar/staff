include ../../Makefile.env

TARGET = staff_security_module
TARGETNAME = lib$(TARGET).so

DEPLOYLIBDIR = $(DEPLOYDIR)../axis2c/modules/staff_security/
INSTALLLIBDIR = $(AXIS2C_HOME)/modules/staff_security/

CFLAGS += -c -I$(DEPLOYDIR)include -I$(AXIS2C_INCDIR)
LFLAGS += -L$(DEPLOYDIR)lib -L$(AXIS2C_LIBDIR)
LFLAGS += -laxis2_axiom -laxutil -laxis2_engine -lstaffsecurity
LFLAGS += -shared -Wl,-soname,$(TARGETNAME)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.c,$(SRCDIR)))
OBJECTS := $(patsubst %.c,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

# compile
$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" "$(DEPLOYLIBDIR)" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYLIBDIR)$(TARGETNAME)
	cp config/module.xml $(DEPLOYLIBDIR)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLLIBDIR)" "$(INSTALLDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLLIBDIR)$(TARGETNAME)
	cp config/module.xml $(INSTALLLIBDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLLIBDIR)$(TARGETNAME)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
