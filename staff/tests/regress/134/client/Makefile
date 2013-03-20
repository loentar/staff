include ../../../../Makefile.env

INTERFACES := Echo.h
TARGET = staff_test_regress_$(notdir $(patsubst %/,%,$(dir $(CURDIR))))

CXXFLAGS += -c -I$(DEPLOYDIR)$(INCDIR) -I $(AXIS2C_HOME)/include/axis2-*
LDFLAGS += -L$(AXIS2C_HOME)/lib -laxutil -laxis2_engine -L$(DEPLOYDIR)$(LIBDIR) -lstaffutils -lstaffcommon -lstaffclient

CODEGEN_INTERFACES := $(patsubst %.h,$(SRCDIR)%.h,$(INTERFACES))
CODEGEN_HEADERS := $(patsubst %.h,%Proxy.h,$(CODEGEN_INTERFACES))
CODEGEN_SOURCES := $(patsubst %.h,%Proxy.cpp,$(CODEGEN_INTERFACES))
CODEGEN_FILES := $(CODEGEN_HEADERS) $(CODEGEN_SOURCES)

NON_CODEGEN_SOURCES := $(filter-out $(CODEGEN_SOURCES),$(wildcard $(SRCDIR)*.cpp))
SOURCES := $(CODEGEN_SOURCES) $(NON_CODEGEN_SOURCES)
OBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(SOURCES))

.NOTPARALLEL: $(CODEGEN_FILES)

# == make ===========================================
make: checkdeploy "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGET)

# link
$(OUTDIR)$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGET) $(LDFLAGS)

# compile
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NON_CODEGEN_SOURCES): $(CODEGEN_SOURCES)

$(SRCDIR)%Proxy.h $(SRCDIR)%Proxy.cpp:: $(SRCDIR)%.h
	$(STAFF_HOME)/bin/staff_codegen -e -tclient -c$(SRCDIR) $(INTERFACES)

# == clean ==========================================
clean:
	rm -f $(OUTDIR)$(TARGET) $(OBJECTS) $(CODEGEN_FILES)
	test ! -d $(OBJDIR) || rmdir -p $(OBJDIR)
	test ! -d $(OUTDIR) || rmdir -p $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@

# == test ===========================================
test: make
	$(OUTDIR)$(TARGET)

checkdeploy:
	@test -d $(DEPLOYDIR) || (echo "Please compile staff first."; false)
