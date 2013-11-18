include ../../Makefile.env

TARGET = common

CXXFLAGS += -c -I$(DEPLOYDIR)include
ifneq (,$(filter $(OS),MINGW WINSDK))
  CXXFLAGS += -DSTAFF_COMMON_DLL_EXPORTS=1 -DAXIS2_DECLARE_EXPORT
endif
LDFLAGS += $(LDAXIS2LIBS) -lstaffutils
LDFLAGS += $(LDLIB)$(LIBTARGETVER)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

OUTDIR := $(DEPLOYDIR)$(LIBDIR)

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy

# link
$(OUTDIR)$(LIBTARGETVER): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(LIBTARGETVER) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" deploy_libs deploy_headers

deploy_libs: $(OUTDIR)$(LIBTARGETVER)
	$(LN) $(LIBTARGETVER) $(OUTDIR)$(LIBTARGET)

deploy_headers: "$(DEPLOYDIR)$(INCDIR)"
	cp -f $(HEADERS) $(DEPLOYDIR)$(INCDIR)
	find  $(DEPLOYDIR)$(INCDIR) -type d | xargs chmod a+x
	$(LN) staff-$(VERSION) $(DEPLOYDIR)$(INCDIR_STAFF)

# == distrib =========================================
distrib:;

# == install ========================================
install: check install_libs install_headers

install_libs: "$(INSTALLDIR)$(LIBDIR)"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)$(LIBTARGETVER)
ifeq ($(OS),WINSDK)
	cp -f $(OUTDIR)$(LIBTARGET_BASE).lib $(OUTDIR)$(LIBTARGET_BASE).exp $(INSTALLDIR)$(LIBDIR)
endif
	$(LN) $(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)$(LIBTARGET)

install_headers: "$(INSTALLDIR)$(INCDIR)"
	cp -f $(HEADERS) $(INSTALLDIR)$(INCDIR)
	find  $(INSTALLDIR)$(INCDIR) -type d | xargs chmod a+x
	$(LN) staff-$(VERSION) $(INSTALLDIR)$(INCDIR_STAFF)

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
