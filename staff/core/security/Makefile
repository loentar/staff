include ../../Makefile.env

TARGET = security
TARGETNAME = libstaff$(TARGET).so

CFLAGS += -c -D_DEBUG -DDEBUG -I/usr/include/postgresql -Wall -D_REENTRANT
LFLAGS += -shared -Wl,-soname,$(TARGETNAME)
LFLAGS += -lpq -lpthread

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.c,$(SRCDIR)))
OBJECTS := $(patsubst %.c,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

# compile
$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" deploy_libs deploy_headers

deploy_libs: "$(DEPLOYDIR)$(LIBDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYDIR)$(LIBDIR)$(TARGETNAME)

deploy_headers: "$(DEPLOYDIR)$(INCDIR)"
	cp -f $(HEADERS) $(DEPLOYDIR)$(INCDIR)
	find  $(DEPLOYDIR)$(INCDIR) -type d | xargs chmod a+x

# == install ========================================
install: check "$(INSTALLDIR)" install_libs install_headers

install_libs: "$(INSTALLDIR)$(LIBDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(LIBDIR)$(TARGETNAME)

install_headers: "$(INSTALLDIR)$(INCDIR)"
	cp -f $(HEADERS) $(INSTALLDIR)$(INCDIR)
	find  $(INSTALLDIR)$(INCDIR) -type d | xargs chmod a+x

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)$(TARGETNAME)
	rm -Rf $(INSTALLDIR)$(INCDIR)

# == install db =====================================
installdb: uninstalldb
	psql template1 < db/instdb.sql
#	psql staff < db/staff.sql 

# == uninstall db =====================================
uninstalldb:
	-psql template1 < db/uninstdb.sql

# == dump db ========================================
dumpdb:
	pg_dump -Fp staff -fdb/staff.sql 

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
