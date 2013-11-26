include ../../../Makefile.env

TARGET = protobuf
LIBTARGETVER = $(LIBPREFIX)staffcgparser-$(TARGET)$(LIBVEREXT)
LIBTARGET = $(LIBPREFIX)staffcgparser-$(TARGET)$(LIBEXT)

CXXFLAGS += -c -I$(DEPLOYDIR)include
LDFLAGS += -lstaffutils -lstaffcodegenparser $(HOSTLDFLAGS)
LDFLAGS += $(LDLIB)$(LIBTARGETVER)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.NOTPARALLEL: $(OUTDIR)$(LIBTARGETVER) deploy

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy

# link
$(OUTDIR)$(LIBTARGETVER): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(LIBTARGETVER) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

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

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)codegen/parsers/$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)codegen/parsers/$(LIBTARGET)
	-cd $(INSTALLDIR) && rmdir -p $(LIBDIR)codegen/parsers/

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
