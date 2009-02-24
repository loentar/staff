include ../../Makefile.env

TARGET = staff_module
TARGETNAME = lib$(TARGET).so

DEPLOYLIBDIR = $(DEPLOYDIR)axis2/modules/staff/
INSTALLLIBDIR = $(AXIS2C_HOME)/modules/staff/

CXXFLAGS += -c -Wno-deprecated -fexceptions -I$(DEPLOYDIR)include -I$(AXIS2C_INCDIR) -I$(STAFF_HOME)/include/
LDFLAGS += -fexceptions -L$(DEPLOYDIR)lib -L$(AXIS2C_LIBDIR) -L$(STAFF_HOME)/lib
LDFLAGS += -lstaffcommon -lstaffcomponent -lrise
LDFLAGS += -laxis2_axiom -laxutil -laxis2_engine
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
deploy: "$(DEPLOYDIR)" "$(DEPLOYLIBDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYLIBDIR)$(TARGETNAME)
	cp config/module.xml $(DEPLOYLIBDIR)

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
