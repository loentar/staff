include ../../../Makefile.env

TARGET = core_security
TARGETNAME = staff_test_$(TARGET)

# == 
CXXFLAGS += -c -I$(DEPLOYDIR)$(INCDIR)
LDFLAGS += -lstaffutils -lstaffcommon -lstaffsecurity -L$(DEPLOYDIR)$(LIBDIR)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: checkdeploy "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME)

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LDFLAGS)

$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

install:;
uninstall:;
distrib:;


# == test ===========================================
test: make
	unset STAFF_LOG_LEVEL; LD_LIBRARY_PATH="$(LD_LIBRARY_PATH):$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(TARGETNAME)

checkdeploy:
	@test -d $(DEPLOYDIR) || (echo "Please compile staff first."; false)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
