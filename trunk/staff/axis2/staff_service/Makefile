include ../../Makefile.env

TARGET = staff_service
LIBTARGETVER = $(LIBPREFIX)$(TARGET)$(LIBVEREXT)
LIBTARGET = $(LIBPREFIX)$(TARGET)$(LIBEXT)

INSTALLLIBDIR = $(AXIS2C_HOME)/services/staff/

DEPLOYLIBDIR = $(DEPLOYROOTDIR)$(AXIS2C_HOME)/services/staff/

CXXFLAGS += -DVERSION_FULL=\"$(VERSION_FULL)\"
CXXFLAGS += -c -I$(DEPLOYDIR)include
LDFLAGS += -L$(DEPLOYDIR)lib -L$(STAFF_HOME)/lib $(LDAXIS2LIBS)
LDFLAGS += -lstaffutils -lstaffcommon -lstaffcomponent
LDFLAGS += $(LDLIB)$(LIBTARGETVER)

ifeq ($(WITHOUT_SECURITY),)
LDFLAGS += -lstaffsecurity
endif

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy

# link
$(OUTDIR)$(LIBTARGETVER): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(LIBTARGETVER) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" "$(DEPLOYLIBDIR)" $(OUTDIR)$(LIBTARGETVER)
	cp -f $(OUTDIR)$(LIBTARGETVER) $(DEPLOYLIBDIR)$(LIBTARGETVER)
	$(LN) $(LIBTARGETVER) $(DEPLOYLIBDIR)$(LIBTARGET)
	cp -f config/services.xml $(DEPLOYLIBDIR)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLLIBDIR)" "$(INSTALLDIR)"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLLIBDIR)$(LIBTARGETVER)
	$(LN) $(LIBTARGETVER) $(INSTALLLIBDIR)$(LIBTARGET)
	test -e $(INSTALLLIBDIR)/services.xml || cp -f config/services.xml $(INSTALLLIBDIR)/services.xml
ifneq ($(WITHOUT_SECURITY),)
	sed -i '/"staff_security"/{d;q}' $(INSTALLLIBDIR)services.xml
	-rm -rf $(AXIS2C_HOME)/modules/staff_security/
endif

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLLIBDIR)$(LIBTARGETVER) $(INSTALLLIBDIR)$(LIBTARGET)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
