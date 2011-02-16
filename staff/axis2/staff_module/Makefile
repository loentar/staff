include ../../Makefile.env

TARGET = staff_module
TARGETNAME = $(LIBPREFIX)$(TARGET)$(LIBVEREXT)

DEPLOYLIBDIR = $(DEPLOYDIR)../axis2c/modules/staff/
INSTALLLIBDIR = $(AXIS2C_HOME)/modules/staff/

CFLAGS += -c -fexceptions -I$(AXIS2C_INCDIR)
LFLAGS += -fexceptions -L$(AXIS2C_LIBDIR)
LFLAGS += -laxutil -laxis2_engine
ifeq ($(OS),MINGW) # axis2/c library naming bug?
  LFLAGS += -laxiom
else
  LFLAGS += -laxis2_axiom
endif
LFLAGS += $(LDLIB)$(TARGETNAME)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.c,$(SRCDIR)))
OBJECTS := $(patsubst %.c,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LFLAGS)

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
	cp -f config/module.xml $(INSTALLLIBDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLLIBDIR)$(TARGETNAME)
	rm -Rf $(INSTALLINCDIR)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
