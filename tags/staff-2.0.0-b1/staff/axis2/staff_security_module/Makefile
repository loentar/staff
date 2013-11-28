include ../../Makefile.env

TARGET = staff_security_module
LIBTARGETVER = $(LIBPREFIX)$(TARGET)$(LIBVEREXT)
LIBTARGET = $(LIBPREFIX)$(TARGET)$(LIBEXT)

DEPLOYLIBDIR = $(DEPLOYROOTDIR)$(AXIS2C_HOME)/modules/staff_security/
INSTALLLIBDIR = $(AXIS2C_HOME)/modules/staff_security/

CFLAGS += -c -I$(DEPLOYDIR)include -I$(AXIS2C_INCDIR)
CFLAGS += -DLIBRARY_VEREXT=\"$(LIBVEREXT)\"
LFLAGS += -L$(DEPLOYDIR)lib -L$(AXIS2C_LIBDIR) $(LDAXIS2LIBS)
ifneq ($(OS),FreeBSD)
ifeq (,$(filter $(OS),MINGW WINSDK))
  LFLAGS += -ldl
endif
ifeq ($(OS),WINSDK)
  CFLAGS += -DWINSDK
endif
endif

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

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
