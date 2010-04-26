include ../../Makefile.env

TARGET = staff_security_test
TARGETNAME = $(TARGET)

# == 
CXXFLAGS += -c -Wall -D_REENTRANT
LDFLAGS += -lpthread -lstaffcommon -lstaffsecurity -lrise 

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME)

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

install:;
uninstall:;
distrib:;

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
