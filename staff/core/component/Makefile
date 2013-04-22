include ../../Makefile.env

TARGET = component

CXXFLAGS += -c -I$(DEPLOYDIR)include
ifeq ($(OS),MINGW)
  CXXFLAGS += -DSTAFF_COMPONENT_DLL_EXPORTS=1
endif

LDFLAGS += -L$(DEPLOYDIR)lib -L$(STAFF_HOME)/lib
LDFLAGS += -lstaffutils -lstaffxml -lstaffcommon
LDFLAGS += $(LDLIB)$(LIBTARGETVER)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
ifneq ($(WITHOUT_SECURITY),)
HEADERS := $(filter-out $(SRCDIR)SessionManager.h,$(HEADERS))
SOURCES := $(filter-out $(SRCDIR)SessionManager.cpp,$(SOURCES))
else
LDFLAGS += -lstaffsecurity
endif
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

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

deploy_libs: "$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(LIBTARGETVER)
	cp -f $(OUTDIR)$(LIBTARGETVER) $(DEPLOYDIR)$(LIBDIR)$(LIBTARGETVER)
	$(LN) $(LIBTARGETVER) $(DEPLOYDIR)$(LIBDIR)$(LIBTARGET)

deploy_headers: "$(DEPLOYDIR)$(INCDIR)"
	cp -f $(HEADERS) $(DEPLOYDIR)$(INCDIR)
	find  $(DEPLOYDIR)$(INCDIR) -type d | xargs chmod a+x
	$(LN) staff-$(VERSION) $(DEPLOYDIR)$(INCDIR_STAFF)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)" install_libs install_headers

install_libs: "$(INSTALLDIR)$(LIBDIR)"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)$(LIBTARGETVER)
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
