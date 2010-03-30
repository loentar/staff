include ../../Makefile.env


TARGET = security
TARGETNAME = libstaff$(TARGET).so

CFLAGS += -c -Wall -D_REENTRANT -DSQLITE_THREADSAFE=1
LFLAGS += -shared -Wl,-soname,$(TARGETNAME)
LFLAGS += -lpthread -ldl -lstaffcommon -lrise

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.c,$(SRCDIR))) $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.PHONY: db

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

# compile
$(OBJDIR)%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)%.cpp.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" deploy_libs deploy_headers deploy_db

deploy_libs: "$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYDIR)$(LIBDIR)$(TARGETNAME)

deploy_headers: "$(DEPLOYDIR)$(INCDIR)"
	cp -f $(HEADERS) $(DEPLOYDIR)$(INCDIR)
	find $(DEPLOYDIR)$(INCDIR) -type d | xargs chmod a+x

deploy_db: "$(DEPLOYDIR)db/"
	cp -f db/*.db $(DEPLOYDIR)db/

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)" install_libs install_headers install_db

install_libs: "$(INSTALLDIR)$(LIBDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(LIBDIR)$(TARGETNAME)

install_headers: "$(INSTALLDIR)$(INCDIR)"
	cp -f $(HEADERS) $(INSTALLDIR)$(INCDIR)
	find  $(INSTALLDIR)$(INCDIR) -type d | xargs chmod a+x

install_db: "$(INSTALLDIR)db/"
	cp -f db/staff.db $(INSTALLDIR)db/
	chmod go+rw -R $(INSTALLDIR)db/

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)$(TARGETNAME)
	rm -Rf $(INSTALLDIR)$(INCDIR)
	rm -Rf $(INSTALLDIR)db/

# == make db ========================================
db:
	rm -f db/staff.db
	sqlite3 db/staff.db < db/staff.sql

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
