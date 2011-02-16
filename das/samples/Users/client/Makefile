include ../../../Makefile.env

TARGET = das_users_client
TARGETNAME = $(TARGET)

DATASOURCES = staff.das.samples.Users staff.das.samples.Groups
SERVICES_HEADERS := $(SRCDIR)Users.h $(SRCDIR)Groups.h $(SRCDIR)Role.h

STAFF_CODEGEN := $(STAFF_HOME)/bin/staff_codegen

CXXFLAGS += -Wno-deprecated -fexceptions -I$(STAFF_HOME)/include/
LDFLAGS += -fexceptions -L$(STAFF_HOME)/lib
LDFLAGS += -lstaffcommon -lstaffclient -lrise

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))


PROXIESH := $(patsubst %.h,%Proxy.h,$(SERVICES_HEADERS))
PROXIESCPP := $(patsubst %.h,%Proxy.cpp,$(SERVICES_HEADERS))
CODEGENH := $(SERVICES_HEADERS) $(PROXIESH)
CODEGENCPP :=  $(PROXIESCPP)
CODEGEN := $(CODEGENH) $(CODEGENCPP)

EXISTING_SERVICE_HEADERS := $(filter $(SERVICES_HEADERS),$(wildcard $(SERVICES_HEADERS)))

SOURCES = $(CODEGENCPP) $(filter-out %Proxy.cpp,$(wildcard $(SRCDIR)*.cpp))
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.SECONDARY: $(CODEGEN)

# == make ===========================================
make: check codegen "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

codegen:
	[ \! -z "$(EXISTING_SERVICE_HEADERS)" ] || ($(STAFF_CODEGEN) -pdas -tinterface -n$(TARGET) -osrc $(DATASOURCES) && sleep 1)
	cd src && $(STAFF_CODEGEN) -u $(patsubst $(SRCDIR)%,%,$(SERVICES_HEADERS)) -tclient -n$(TARGET)

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(SAMPLESDIR)$(BINDIR)" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) "$(DEPLOYDIR)$(SAMPLESDIR)$(BINDIR)"

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(SAMPLESDIR)$(BINDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(SAMPLESDIR)$(BINDIR)$(TARGETNAME)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(SAMPLESDIR)$(BINDIR)$(TARGETNAME)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)
	rm -f $(CODEGEN)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
