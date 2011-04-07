include ../../Makefile.env

POSTGRESQL_DEV_INSTALLED := $(shell whereis postgresql pgsql 2>/dev/null | grep include -c)

COMPONENT = staff.das/providers/staff.das.Postgres
TARGET = postgres
TARGETNAME = $(LIBPREFIX)staffdasprov-$(TARGET)$(LIBVEREXT)

CXXFLAGS += -I$(DEPLOYDIR)include -I/usr/include/postgresql -I/usr/include/pgsql
LDFLAGS += -lpthread -lpq -lrise -lstaffcommon -lstaffdascommon
LDFLAGS += $(LDLIB)$(TARGETNAME)

SOURCES = $(wildcard $(SRCDIR)*.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
ifneq ($(POSTGRESQL_DEV_INSTALLED),0)
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy
else
make:
	@$(ECHO) "\n\033[33;1mSkipping Postgresql provider compilation because Postgresql development package is not installed.\033[31;0m\n"
endif

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
ifneq ($(POSTGRESQL_DEV_INSTALLED),0)
install: check "$(INSTALLDIR)$(COMDIR)$(COMPONENT)/"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(COMDIR)$(COMPONENT)/
else
install:;
endif

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
