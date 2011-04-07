include ../../Makefile.env

TARGET = utils
TARGETNAME = $(LIBPREFIX)staff$(TARGET)$(LIBVEREXT)

CXXFLAGS += -c -I$(DEPLOYDIR)include -I$(STAFF_HOME)/include/
ifeq ($(OS),MINGW)
  CXXFLAGS += -DSTAFF_UTILS_DLL_EXPORTS=1
endif
LDFLAGS += -lrise
LDFLAGS += $(LDLIB)$(TARGETNAME)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" deploy_libs deploy_headers

deploy_libs: "$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYDIR)$(LIBDIR)$(TARGETNAME)

deploy_headers: "$(DEPLOYDIR)$(INCDIR)"
	cp -f $(HEADERS) $(DEPLOYDIR)$(INCDIR)
	find  $(DEPLOYDIR)$(INCDIR) -type d | xargs chmod a+x

# == distrib =========================================
distrib:;

# == install ========================================
install: check install_libs install_headers

install_libs: "$(INSTALLDIR)$(LIBDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(LIBDIR)$(TARGETNAME)

install_headers: "$(INSTALLDIR)$(INCDIR)"
	cp -f $(HEADERS) $(INSTALLDIR)$(INCDIR)
	find  $(INSTALLDIR)$(INCDIR) -type d | xargs chmod a+x

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)$(TARGETNAME)
	rm -Rf $(INSTALLDIR)$(INCDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
