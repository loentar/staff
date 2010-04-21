include ../../Makefile.env

COMPONENT = webapp.admin
TARGET = ProfileAdmin
TARGETNAME = lib$(TARGET).so

CXXFLAGS += -fPIC -fexceptions -D_REENTRANT -Wall
LDFLAGS += -fPIC -fexceptions -Wl,-rpath,$(AXIS2C_LIBDIR)

VPATH = $(SRCDIR)

INSTALLDIR = $(STAFF_HOME)/components/

CXXFLAGS += -Wno-deprecated -I$(STAFF_DEPLOY_HOME)/include/
LDFLAGS += -L$(STAFF_DEPLOY_HOME)/lib
LDFLAGS += -lstaffcommon -lstaffcomponent -lstaffsecurity -lrise -lpthread
LDFLAGS += -shared -Wl,-soname,$(TARGETNAME)

COMPONENT_SERVICES := $(patsubst $(SRCDIR)%Impl.h, %, $(filter-out $(SRCDIR)ComponentImpl.h, $(wildcard $(SRCDIR)*Impl.h)))
COMPONENT_SERVICES_HEADERS := $(patsubst %, $(SRCDIR)%.h, $(COMPONENT_SERVICES))

WSDLS := $(patsubst %, $(OUTDIR)$(COMPONENT).%.wsdl, $(COMPONENT_SERVICES))

COMPONENT_SERVICES_CODEGEN := $(SRCDIR)ComponentImpl.h $(SRCDIR)ComponentImpl.cpp \
    $(patsubst %.h,%Wrapper.h,$(COMPONENT_SERVICES_HEADERS)) $(patsubst %.h,%Wrapper.cpp,$(COMPONENT_SERVICES_HEADERS))

CODEGEN_SOURCES := $(filter %.cpp,$(COMPONENT_SERVICES_CODEGEN))

SOURCES = $(filter-out $(CODEGEN_SOURCES) ,$(wildcard $(SRCDIR)*.cpp)) $(CODEGEN_SOURCES)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.NOTPARALLEL: $(CODEGEN_SOURCES)

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" codegen $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

# compile
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# == codegen ===========================================
codegen: $(COMPONENT_SERVICES_CODEGEN) $(WSDLS)

$(SRCDIR)%Wrapper.h $(SRCDIR)%Wrapper.cpp:: $(SRCDIR)%.h
	cd $(SRCDIR) && $(STAFF_CODEGEN) -u -tcomponent $(patsubst $(SRCDIR)%,%,$(COMPONENT_SERVICES_HEADERS))

$(SRCDIR)ComponentImpl.h $(SRCDIR)ComponentImpl.cpp:: $(COMPONENT_SERVICES_HEADERS)
	cd $(SRCDIR) && $(STAFF_CODEGEN) -u -tcomponent $(patsubst $(SRCDIR)%,%,$(COMPONENT_SERVICES_HEADERS))

$(OUTDIR)$(COMPONENT).%.wsdl: $(SRCDIR)%.h
	$(STAFF_CODEGEN) -u -twsdl $(patsubst $(SRCDIR)%,%,$<) -i$(SRCDIR) -o$(OUTDIR)

# == deploy  ========================================
deploy: "$(DEPLOYDIR)usr/local/staff/components/$(COMPONENT)/" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYDIR)usr/local/staff/components/$(COMPONENT)/

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(TARGET)/" "$(INSTALLDIR)$(COMPONENT)/config/"
	cp -f $(OUTDIR)$(TARGETNAME) "$(INSTALLDIR)$(COMPONENT)/"
	cp -f $(patsubst $(SRCDIR)%, $(OUTDIR)%, $(WSDLS)) $(INSTALLDIR)$(COMPONENT)/

# == uninstall ======================================
uninstall:
	rm -f $(INSTALLDIR)components/$(TARGET)/$(TARGETNAME)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)
	rm -f $(COMPONENT_SERVICES_CODEGEN)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
