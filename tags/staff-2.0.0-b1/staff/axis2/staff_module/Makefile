include ../../Makefile.env

TARGET = staff_module
LIBTARGETVER = $(LIBPREFIX)$(TARGET)$(LIBVEREXT)
LIBTARGET = $(LIBPREFIX)$(TARGET)$(LIBEXT)

DEPLOYLIBDIR = $(DEPLOYROOTDIR)$(AXIS2C_HOME)/modules/staff/
INSTALLLIBDIR = $(AXIS2C_HOME)/modules/staff/

CFLAGS += -c -I$(AXIS2C_INCDIR)
LFLAGS += -L$(AXIS2C_LIBDIR) $(LDAXIS2LIBS)
LFLAGS += $(LDLIB)$(LIBTARGETVER)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.c,$(SRCDIR)))
OBJECTS := $(patsubst %.c,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy

# link
$(OUTDIR)$(LIBTARGETVER): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(OUTDIR)$(LIBTARGETVER) $(LFLAGS)

# compile
$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" "$(DEPLOYLIBDIR)" $(OUTDIR)$(LIBTARGETVER)
	cp -f $(OUTDIR)$(LIBTARGETVER) $(DEPLOYLIBDIR)$(LIBTARGETVER)
	$(LN) $(LIBTARGETVER) $(DEPLOYLIBDIR)$(LIBTARGET)
	cp config/module.xml $(DEPLOYLIBDIR)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLLIBDIR)" "$(INSTALLDIR)"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLLIBDIR)$(LIBTARGETVER)
	$(LN) $(LIBTARGETVER) $(INSTALLLIBDIR)$(LIBTARGET)
	cp -f config/module.xml $(INSTALLLIBDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLLIBDIR)$(LIBTARGETVER) $(INSTALLLIBDIR)$(LIBTARGET)
	rm -Rf $(INSTALLINCDIR)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
