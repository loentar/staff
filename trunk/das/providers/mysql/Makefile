include ../../Makefile.env

MYSQL_DEV_INSTALLED := $(shell whereis mysql | grep include -c)

COMPONENT = staff.das/providers/staff.das.MySql
TARGET = mysql
TARGETNAME = libstaffdasprov-$(TARGET).so

CXXFLAGS += -Wno-deprecated -fexceptions -I$(DEPLOYDIR)include -I/usr/include/mysql
LDFLAGS += -fexceptions -lpthread -lmysqlclient -lrise -lstaffcommon -lstaffdascommon
LDFLAGS += -shared -Wl,-soname,$(TARGETNAME)

SOURCES = $(wildcard $(SRCDIR)*.cpp)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
ifneq ($(MYSQL_DEV_INSTALLED),0)
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy
else
make:
	@/bin/echo -e "\n\033[33;1mSkipping MySQL provider compilation because MySQL development package is not installed.\033[31;0m\n"
endif

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

# compile
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(COMDIR)$(COMPONENT)/" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) "$(DEPLOYDIR)$(COMDIR)$(COMPONENT)/"

# == distrib =========================================
distrib:;

# == install ========================================
ifneq ($(MYSQL_DEV_INSTALLED),0)
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
