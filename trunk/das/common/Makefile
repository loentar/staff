include ../Makefile.env

TARGET = dascommon
TARGETNAME = libstaff$(TARGET)$(LIBVEREXT)

CXXFLAGS += -Wno-deprecated -fexceptions -I$(DEPLOYDIR)include
LDFLAGS += -fexceptions
LDFLAGS += -lstaffcommon -lstaffcomponent -lrise -lpthread
LDFLAGS += $(LDLIB)$(TARGETNAME)

SOURCES = $(wildcard $(SRCDIR)*.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

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

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
