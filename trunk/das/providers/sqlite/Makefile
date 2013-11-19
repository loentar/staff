include ../../Makefile.env

COMPONENT = staff.das/providers/staff.das.Sqlite
TARGET = sqlite
LIBTARGET_BASE = $(LIBPREFIX)staffdasprov-$(TARGET)

ifeq ($(WITHOUT_SECURITY),)
LDFLAGS += -lstaffsqlite3
else
SQLITE_INCLUDE="\#include <sqlite3.h>"
WITH_SQLITE := $(shell echo $(SQLITE_INCLUDE) | LANG=C `$(CC) -print-prog-name=cc1plus` $(filter -I%,$(CXXFLAGS)) >/dev/null 2>&1 && echo "-DWITH_SQLITE"; rm -f gccdump.s)
ifneq ($(WITH_SQLITE),)
CXXFLAGS += $(WITH_SQLITE)
LDFLAGS += -lsqlite3
endif
endif

CXXFLAGS += -I$(DEPLOYDIR)include
LDFLAGS += -lstaffutils -lstaffxml -lstaffcommon -lstaffdascommon
LDFLAGS += $(LDLIB)$(LIBTARGETVER)

SOURCES = $(wildcard $(SRCDIR)*.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
ifneq ($(SQLITE_DEV_INSTALLED),0)
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy
else
make:
	@$(ECHO) "\n\033[33;1mSkipping sqlite provider compilation because MySQL development package is not installed.\033[31;0m\n"
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
ifneq ($(SQLITE_DEV_INSTALLED),0)
install: check "$(INSTALLDIR)$(COMDIR)$(COMPONENT)/"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLDIR)$(COMDIR)$(COMPONENT)/
ifeq ($(OS),WINSDK)
	cp -f $(OUTDIR)$(LIBTARGET_BASE).lib $(OUTDIR)$(LIBTARGET_BASE).exp $(INSTALLDIR)$(COMDIR)$(COMPONENT)/
endif
	$(LN) $(LIBTARGETVER) $(INSTALLDIR)$(COMDIR)$(COMPONENT)/$(LIBTARGET)
else
install:;
endif

# == uninstall ======================================
ifneq ($(SQLITE_DEV_INSTALLED),0)
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
