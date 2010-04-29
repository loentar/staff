include ../../Makefile.env

TARGET = codegen
TARGETNAME = staff_$(TARGET)

HEADERS = $(shell find src *.h | grep -v Proxy | grep -v Factory)

CXXFLAGS += -Wno-deprecated -fexceptions -I$(DEPLOYDIR)include -I$(STAFF_HOME)/include/
LDFLAGS += -fexceptions -L$(DEPLOYDIR)lib -L$(STAFF_HOME)/lib
LDFLAGS += -lrise

VPATH = $(SRCDIR)

SOURCES = $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check $(CODEGEN) "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(BINDIR)" "$(DEPLOYDIR)$(INCDIR)" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYDIR)$(BINDIR)$(TARGETNAME)
	cp -f --parents $$(find template -type f | grep -v '.svn') $(DEPLOYDIR)$(BINDIR)
	cp -f $(SRCDIR)Interface.h $(SRCDIR)CodegenParser.h $(DEPLOYDIR)$(INCDIR)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(BINDIR)" "$(INSTALLDIR)$(INCDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(BINDIR)$(TARGETNAME)
	cp --parents $$(find template -type f | grep -v '.svn') $(INSTALLDIR)$(BINDIR)
	cp -f $(SRCDIR)Interface.h $(SRCDIR)CodegenParser.h $(INSTALLDIR)$(INCDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(BINDIR)$(TARGETNAME)
	rm -Rf $(INSTALLDIR)$(BINDIR)template

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
