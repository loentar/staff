include ../../../Makefile.env

TARGET = das
LIBTARGETVER = $(LIBPREFIX)staffcgparser-$(TARGET)$(LIBVEREXT)
LIBTARGET = $(LIBPREFIX)staffcgparser-$(TARGET)$(LIBEXT)

CXXFLAGS += -c -I$(DEPLOYDIR)include
LDFLAGS += -lstaffcommon -lstaffclient -lrise -lstaffcodegenparser
LDFLAGS += $(LDLIB)$(LIBTARGETVER)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

SERVICES_HEADERS := $(SRCDIR)DataAccessService.h

CODEGENH := $(patsubst %.h,%Proxy.h,$(SERVICES_HEADERS))
CODEGENCPP := $(patsubst %.h,%Proxy.cpp,$(SERVICES_HEADERS))
CODEGEN := $(CODEGENH) $(CODEGENCPP)

SOURCES = $(CODEGENCPP) $(filter-out %Proxy.cpp,$(wildcard $(SRCDIR)*.cpp))
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.NOTPARALLEL: $(OUTDIR)$(LIBTARGETVER) deploy
.NOTPARALLEL: $(CODEGEN)

# == make ===========================================
make: check $(CODEGEN) "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy

# link
$(OUTDIR)$(LIBTARGETVER): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(LIBTARGETVER) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(SRCDIR)%Proxy.h $(SRCDIR)%Proxy.cpp:: $(SRCDIR)%.h
	cd src && $(STAFF_CODEGEN) -u $(patsubst $(SRCDIR)%,%,$(SERVICES_HEADERS)) -tclient -n$(TARGET)

# == deploy ========================================
deploy: "$(DEPLOYDIR)$(LIBDIR)codegen/parsers/"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(DEPLOYDIR)$(LIBDIR)codegen/parsers/
	$(LN) $(LIBTARGETVER) $(DEPLOYDIR)$(LIBDIR)codegen/parsers/$(LIBTARGET)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(LIBDIR)codegen/parsers/"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)codegen/parsers/
	$(LN) $(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)codegen/parsers/$(LIBTARGET)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)
	rm -f $(CODEGEN)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)codegen/parsers/$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)codegen/parsers/$(LIBTARGET)
	-cd $(INSTALLDIR) && rmdir -p $(LIBDIR)codegen/parsers/

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
