include ../Makefile.env

TARGET = dascommon
TARGETNAME = $(LIBPREFIX)staff$(TARGET)$(LIBVEREXT)

CXXFLAGS += -I$(DEPLOYDIR)include
ifeq ($(OS),MINGW)
  CXXFLAGS += -DSTAFF_DAS_COMMON_DLL_EXPORTS
endif
LDFLAGS += -lstaffcommon -lstaffcomponent -lrise -lpthread
LDFLAGS += $(LDLIB)$(TARGETNAME)

SOURCES = $(wildcard $(SRCDIR)*.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LDFLAGS)

# compile
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(TARGETNAME) "$(DEPLOYDIR)include/staff/das/common/"
	cp -f $(SRCDIR)*.h $(DEPLOYDIR)include/staff/das/common/
	cp -f $(OUTDIR)$(TARGETNAME) "$(DEPLOYDIR)$(LIBDIR)"

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(LIBDIR)" "$(STAFF_HOME)/include/staff/das/common/"
	cp -f $(SRCDIR)*.h $(STAFF_HOME)/include/staff/das/common/
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(LIBDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)$(TARGETNAME)
	-rmdir $(INSTALLDIR)$(COMDIR)$(COMPONENT)
	rm -Rf $(STAFF_HOME)/include/staff/das/common/
	-cd $(STAFF_HOME) && rmdir -p include/staff/das/

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
