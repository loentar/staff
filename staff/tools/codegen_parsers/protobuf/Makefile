include ../../../Makefile.env

TARGET = protobuf
TARGETNAME = $(LIBPREFIX)staffcgparser-$(TARGET)$(LIBVEREXT)

CXXFLAGS += -c -I$(DEPLOYDIR)include
LDFLAGS += -lrise -lstaffcodegenparser
LDFLAGS += $(LDLIB)$(TARGETNAME)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.NOTPARALLEL: $(OUTDIR)$(TARGETNAME) deploy

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

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

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)codegen/parsers/$(TARGETNAME)
	-cd $(INSTALLDIR) && rmdir -p $(LIBDIR)codegen/parsers/

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
