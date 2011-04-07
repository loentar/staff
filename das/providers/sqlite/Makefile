include ../../Makefile.env

COMPONENT = staff.das/providers/staff.das.Sqlite
TARGET = sqlite
TARGETNAME = $(LIBPREFIX)staffdasprov-$(TARGET)$(LIBVEREXT)

CXXFLAGS += -I$(DEPLOYDIR)include
LDFLAGS += -lpthread -lstaffsqlite3 -lrise -lstaffcommon -lstaffdascommon
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
deploy: "$(DEPLOYDIR)$(COMDIR)$(COMPONENT)/" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) "$(DEPLOYDIR)$(COMDIR)$(COMPONENT)/"

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(COMDIR)$(COMPONENT)/"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(COMDIR)$(COMPONENT)/

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
