include ../../Makefile.env

TARGET = staff_security_test
TARGETNAME = $(TARGET)

# == 
CFLAGS += -c -Wall -D_REENTRANT
LFLAGS += -lpthread -lstaffcommon -lstaffsecurity -lrise 

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME)

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

$(OBJDIR)%.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

install:;
uninstall:;
distrib:;

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
