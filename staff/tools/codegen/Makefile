include ../../Makefile.env

TARGET = codegen
TARGETNAME = staff_$(TARGET)$(EXEC_EXT)

CXXFLAGS += -I$(DEPLOYDIR)include
CXXFLAGS += -DVERSION_FULL=\"$(VERSION_FULL)\" -DLIBRARY_VERSION=\"$(LIBRARY_VERSION)\"
LDFLAGS += -L$(DEPLOYDIR)lib -L$(STAFF_HOME)/lib
LDFLAGS += -lstaffutils -lstaffxml -lstaffcodegenparser $(HOSTLDFLAGS)


VPATH = $(SRCDIR)

SOURCES = $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

OUTDIR := $(DEPLOYDIR)$(BINDIR)

# == make ===========================================
make: check $(CODEGEN) "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXXHOST) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXXHOST) $(CXXFLAGS) -c $< -o $@

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(INCDIR)" "$(DEPLOYDIR)$(LIBDIR)codegen/"
	cp -Rf templates $(DEPLOYDIR)$(LIBDIR)codegen/
	find $(DEPLOYDIR)$(LIBDIR)codegen/ -type d -name '.svn' | xargs rm -Rf

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(BINDIR)" "$(INSTALLDIR)$(INCDIR)" "$(INSTALLDIR)$(LIBDIR)codegen/"
	@test ! -d $(INSTALLDIR)$(BINDIR)template || ( echo "removing codegen templates from old location..."; rm -Rf $(INSTALLDIR)$(BINDIR)template )
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(BINDIR)$(TARGETNAME)
	ls -d ./templates/* | sed "s/^\./$$(echo $(INSTALLDIR)$(BINDIR) | sed 's/\//\\\//g')/g" | xargs rm -Rf
	cp -Rf templates $(INSTALLDIR)$(LIBDIR)codegen/
	find $(INSTALLDIR)$(LIBDIR)codegen/templates -type d -name '.svn' | xargs rm -Rf

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(BINDIR)$(TARGETNAME)
	rm -Rf $(INSTALLDIR)$(LIBDIR)codegen/template
	-rmdir $(INSTALLDIR)$(BINDIR)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
