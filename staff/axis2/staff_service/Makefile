include ../../Makefile.env

TARGET = staff_service
TARGETNAME = lib$(TARGET).so

INSTALLLIBDIR = $(AXIS2C_HOME)/services/staff/

DEPLOYLIBDIR = $(DEPLOYDIR)../axis2c/services/staff/

CXXFLAGS += -c -Wno-deprecated -fexceptions -I$(DEPLOYDIR)include
LDFLAGS += -fexceptions -L$(DEPLOYDIR)lib -L$(STAFF_HOME)/lib
LDFLAGS += -lstaffcommon -lstaffcomponent -lstaffsecurity -lrise -lpthread -lstdc++
LDFLAGS += -shared -Wl,-soname,$(TARGETNAME)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" "$(DEPLOYLIBDIR)" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYLIBDIR)$(TARGETNAME)
	cp -f config/services.xml $(DEPLOYLIBDIR)
	cp -f config/staff.xml $(DEPLOYDIR)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLLIBDIR)" "$(INSTALLDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLLIBDIR)$(TARGETNAME)
	cp -f config/services.xml $(INSTALLLIBDIR)
	cp -f config/staff.xml $(INSTALLDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLLIBDIR)$(TARGETNAME)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
