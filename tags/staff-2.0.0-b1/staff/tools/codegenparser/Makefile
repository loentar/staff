include ../../Makefile.env

TARGET = codegen
LIBTARGET_BASE = $(LIBPREFIX)staff$(TARGET)parser

ifneq (,$(filter $(OS),MINGW WINSDK))
  CXXFLAGS += -DSTAFF_CODEGENPARSER_DLL_EXPORTS=1 -DAXIS2_DECLARE_EXPORT
endif

CXXFLAGS += -c -I$(DEPLOYDIR)include
LDFLAGS += $(LDLIB)$(LIBTARGETVER)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

OUTDIR := $(DEPLOYDIR)$(LIBDIR)

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
deploy: "$(DEPLOYDIR)$(LIBDIR)" "$(DEPLOYDIR)$(INCDIR)"
	$(LN) $(LIBTARGETVER) $(DEPLOYDIR)$(LIBDIR)$(LIBTARGET)
	cp -f $(SRCDIR)*.h $(DEPLOYDIR)$(INCDIR)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(LIBDIR)" "$(INSTALLDIR)$(INCDIR)"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)
ifeq ($(OS),WINSDK)
	cp -f $(OUTDIR)$(LIBTARGET_BASE).lib $(OUTDIR)$(LIBTARGET_BASE).exp $(INSTALLDIR)$(LIBDIR)
endif
	$(LN) $(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)$(LIBTARGET)
	cp -f $(SRCDIR)*.h $(INSTALLDIR)$(INCDIR)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)$(LIBTARGET)
	rm -Rf $(INSTALLDIR)$(INCDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
