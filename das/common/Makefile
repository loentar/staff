include ../Makefile.env

TARGET = dascommon

CXXFLAGS += -I$(DEPLOYDIR)include
CXXFLAGS += -DVERSION_FULL=\"$(VERSION_FULL)\" -DLIBRARY_VERSION=\"$(LIBRARY_VERSION)\"
ifneq (,$(filter $(OS),MINGW WINSDK))
  CXXFLAGS += -DSTAFF_DAS_COMMON_DLL_EXPORTS
endif
LDFLAGS += -lstaffutils -lstaffxml -lstaffcommon -lstaffcomponent
LDFLAGS += $(LDLIB)$(LIBTARGETVER)

SOURCES = $(wildcard $(SRCDIR)*.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

OUTDIR := $(DEPLOYDIR)$(LIBDIR)

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy

# link
$(OUTDIR)$(LIBTARGETVER): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(LIBTARGETVER) $(LDFLAGS)

# compile
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(LIBDIR)" "$(DEPLOYDIR)include/staff/das/common/"
	cp -f $(SRCDIR)*.h $(DEPLOYDIR)include/staff/das/common/
	$(LN) $(LIBTARGETVER) "$(DEPLOYDIR)$(LIBDIR)$(LIBTARGET)"

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(LIBDIR)" "$(STAFF_HOME)/include/staff/das/common/"
	cp -f $(SRCDIR)*.h $(STAFF_HOME)/include/staff/das/common/
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)
ifeq ($(OS),WINSDK)
	cp -f $(OUTDIR)$(LIBTARGET_BASE).lib $(OUTDIR)$(LIBTARGET_BASE).exp $(INSTALLDIR)$(LIBDIR)
endif
	$(LN) $(LIBTARGETVER) "$(INSTALLDIR)$(LIBDIR)$(LIBTARGET)"

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)$(LIBTARGET)
	-rmdir $(INSTALLDIR)$(COMDIR)$(COMPONENT)
	rm -Rf $(STAFF_HOME)/include/staff/das/common/
	-cd $(STAFF_HOME) && rmdir -p include/staff/das/

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
