include ../../Makefile.env

TARGET = sqlite3

CFLAGS += -c -DSQLITE_THREADSAFE=1
ifeq ($(OS),MINGW)
  CFLAGS += -DSQLITE_DLL_EXPORTS=1
endif
LFLAGS += $(LDLIB)$(LIBTARGETVER)
ifneq ($(OS),FreeBSD)
ifneq ($(OS),MINGW)
  LFLAGS += -ldl
endif
endif

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.c,$(SRCDIR)))
OBJECTS := $(patsubst %.c,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy

# link
$(OUTDIR)$(LIBTARGETVER): $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $(OUTDIR)$(LIBTARGETVER)

# compile
$(OBJDIR)%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" deploy_libs deploy_headers

deploy_libs: "$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(LIBTARGETVER)
	cp -f $(OUTDIR)$(LIBTARGETVER) $(DEPLOYDIR)$(LIBDIR)$(LIBTARGETVER)
	$(LN) $(LIBTARGETVER) $(DEPLOYDIR)$(LIBDIR)$(LIBTARGET)

deploy_headers: "$(DEPLOYDIR)$(INCDIR)"
	cp -f $(HEADERS) $(DEPLOYDIR)$(INCDIR)
	find $(DEPLOYDIR)$(INCDIR) -type d | xargs chmod a+x
	$(LN) staff-$(VERSION) $(DEPLOYDIR)$(INCDIR_STAFF)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)" install_libs install_headers

install_libs: "$(INSTALLDIR)$(LIBDIR)"
	cp -f $(OUTDIR)$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)$(LIBTARGETVER)
	$(LN) $(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)$(LIBTARGET)

install_headers: "$(INSTALLDIR)$(INCDIR)"
	cp -f $(HEADERS) $(INSTALLDIR)$(INCDIR)
	find  $(INSTALLDIR)$(INCDIR) -type d | xargs chmod a+x
	$(LN) staff-$(VERSION) $(INSTALLDIR)$(INCDIR_STAFF)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(LIBDIR)$(LIBTARGETVER) $(INSTALLDIR)$(LIBDIR)$(LIBTARGET)
	rm -Rf $(INSTALLDIR)$(INCDIR)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
