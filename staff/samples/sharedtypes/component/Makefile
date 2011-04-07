include ../../../Makefile.env

COMPONENT = samples.sharedtypes
TARGET = $(COMPONENT)
TARGETNAME = $(LIBPREFIX)$(TARGET)$(LIBVEREXT)

CXXFLAGS += -I$(STAFF_HOME)/include/
LDFLAGS += -L$(STAFF_HOME)/lib -lpthread -lstaffcommon -lstaffcomponent -lrise
LDFLAGS += $(LDLIB)$(TARGETNAME)

COMPONENT_SERVICES := $(patsubst $(SRCDIR)%Impl.h, %, $(filter-out $(SRCDIR)ComponentImpl.h, $(wildcard $(SRCDIR)*Impl.h)))
COMPONENT_SERVICES_HEADERS := $(patsubst %, $(SRCDIR)%.h, $(COMPONENT_SERVICES)) $(SRCDIR)Ticket.h


WSDLS := $(patsubst %, $(OUTDIR)$(COMPONENT).%.wsdl, $(COMPONENT_SERVICES))

COMPONENT_SERVICES_CODEGEN := $(SRCDIR)ComponentImpl.h $(SRCDIR)ComponentImpl.cpp \
    $(patsubst %.h,%Wrapper.h,$(COMPONENT_SERVICES_HEADERS)) $(patsubst %.h,%Wrapper.cpp,$(COMPONENT_SERVICES_HEADERS))

CODEGEN_SOURCES := $(filter %.cpp,$(COMPONENT_SERVICES_CODEGEN))

SOURCES = $(filter-out $(CODEGEN_SOURCES) ,$(wildcard $(SRCDIR)*.cpp)) $(CODEGEN_SOURCES)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.NOTPARALLEL: $(CODEGEN_SOURCES)

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): codegen $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LDFLAGS)

# compile
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# == codegen ===========================================
codegen: $(COMPONENT_SERVICES_CODEGEN) $(WSDLS)

$(SRCDIR)%Wrapper.h $(SRCDIR)%Wrapper.cpp:: $(SRCDIR)%.h
	cd $(SRCDIR) && $(STAFF_CODEGEN) -tcomponent $(patsubst $(SRCDIR)%,%,$(COMPONENT_SERVICES_HEADERS))

$(SRCDIR)ComponentImpl.h $(SRCDIR)ComponentImpl.cpp:: $(COMPONENT_SERVICES_HEADERS)
	cd $(SRCDIR) && $(STAFF_CODEGEN) -tcomponent $(patsubst $(SRCDIR)%,%,$(COMPONENT_SERVICES_HEADERS))

$(OUTDIR)$(COMPONENT).%.wsdl: $(SRCDIR)%.h
	$(STAFF_CODEGEN) -u -twsdl $(patsubst $(SRCDIR)%,%,$<) -i$(SRCDIR) -o$(OUTDIR)

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(COMDIR)$(COMPONENT)/" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) "$(DEPLOYDIR)$(COMDIR)$(COMPONENT)/"

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(COMDIR)$(COMPONENT)/"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(COMDIR)$(COMPONENT)/
	cp -f $(patsubst $(SRCDIR)%, $(OUTDIR)%, $(WSDLS)) $(OUTDIR)/samples.ticket.Ticket.xsd $(INSTALLDIR)$(COMDIR)$(COMPONENT)/
	chmod 777 $(INSTALLDIR)$(COMDIR)$(COMPONENT)/

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(COMDIR)$(COMPONENT)/$(TARGETNAME)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)
	rm -f $(COMPONENT_SERVICES_CODEGEN)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
