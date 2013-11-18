include ../../Makefile.env

ifneq ($(OS),WINSDK)
POSTGRESQL_DEV_INSTALLED := $(shell $(WHEREISCMD) postgresql pgsql 2>/dev/null | grep -c include)
else
POSTGRESQL_DEV_INSTALLED := 0
endif

COMPONENT = staff.das/providers/staff.das.Postgres
TARGET = postgres
LIBTARGETVER = $(LIBPREFIX)staffdasprov-$(TARGET)$(LIBVEREXT)
LIBTARGET = $(LIBPREFIX)staffdasprov-$(TARGET)$(LIBEXT)

CXXFLAGS += -I$(DEPLOYDIR)include -I/usr/include/postgresql -I/usr/include/pgsql
LDFLAGS += -lpq -lstaffutils -lstaffxml -lstaffcommon -lstaffdascommon
LDFLAGS += $(LDLIB)$(LIBTARGETVER)

SOURCES = $(wildcard $(SRCDIR)*.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
ifeq ($(CROSSCOMPILE),0)
ifneq ($(POSTGRESQL_DEV_INSTALLED),0)
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy
else
make:
	@$(ECHO) "\n\033[33;1mSkipping Postgresql provider compilation because Postgresql development package is not installed.\033[31;0m\n"
endif
else
POSTGRESQL_DEV_INSTALLED=0
make:
	@$(ECHO) "Crosscompilation of postgresql provider is not supported now"
endif

# link
$(OUTDIR)$(LIBTARGETVER): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(LIBTARGETVER) $(LDFLAGS)

# compile
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(COMDIR)$(COMPONENT)/" $(OUTDIR)$(LIBTARGETVER)
	cp -f $(OUTDIR)$(LIBTARGETVER) "$(DEPLOYDIR)$(COMDIR)$(COMPONENT)/"
	$(LN) $(LIBTARGETVER) $(DEPLOYDIR)$(COMDIR)$(COMPONENT)/$(LIBTARGET)

# == distrib =========================================
distrib:;

# == install ========================================
ifneq ($(POSTGRESQL_DEV_INSTALLED),0)
install: check "$(INSTALLDIR)$(COMDIR)$(COMPONENT)/"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLDIR)$(COMDIR)$(COMPONENT)/
	$(LN) $(LIBTARGETVER) $(INSTALLDIR)$(COMDIR)$(COMPONENT)/$(LIBTARGET)
else
install:;
endif

# == uninstall ======================================
ifneq ($(POSTGRESQL_DEV_INSTALLED),0)
uninstall: check
	rm -f $(INSTALLDIR)$(COMDIR)$(COMPONENT)/$(LIBTARGETVER) $(INSTALLDIR)$(COMDIR)$(COMPONENT)/$(LIBTARGET)
	-rmdir $(INSTALLDIR)$(COMDIR)$(COMPONENT)
else
uninstall:;
endif

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
