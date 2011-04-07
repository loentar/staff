include ../../../Makefile.env

TARGET = das
TARGETNAME = $(LIBPREFIX)staffcgparser-$(TARGET)$(LIBVEREXT)

CXXFLAGS += -c -I$(DEPLOYDIR)include
LDFLAGS += -lstaffcommon -lstaffclient -lrise -lstaffcodegenparser
LDFLAGS += $(LDLIB)$(TARGETNAME)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

SERVICES_HEADERS := $(SRCDIR)DataAccessService.h

CODEGENH := $(patsubst %.h,%Proxy.h,$(SERVICES_HEADERS))
CODEGENCPP := $(patsubst %.h,%Proxy.cpp,$(SERVICES_HEADERS))
CODEGEN := $(CODEGENH) $(CODEGENCPP)

SOURCES = $(CODEGENCPP) $(filter-out %Proxy.cpp,$(wildcard $(SRCDIR)*.cpp))
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.NOTPARALLEL: $(OUTDIR)$(TARGETNAME) deploy
.NOTPARALLEL: $(CODEGEN)

# == make ===========================================
make: check $(CODEGEN) "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(SRCDIR)%Proxy.h $(SRCDIR)%Proxy.cpp:: $(SRCDIR)%.h
	cd src && $(STAFF_CODEGEN) -u $(patsubst $(SRCDIR)%,%,$(SERVICES_HEADERS)) -tclient -n$(TARGET)

# == deploy ========================================
deploy: "$(DEPLOYDIR)$(LIBDIR)codegen/parsers/"
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYDIR)$(LIBDIR)codegen/parsers/

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(LIBDIR)codegen/parsers/"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(LIBDIR)codegen/parsers/

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)
	rm -f $(CODEGEN)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)$(TARGETNAME)
	rm -Rf $(INSTALLDIR)$(INCDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
