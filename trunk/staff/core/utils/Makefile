include ../../Makefile.env

TARGET = utils

CXXFLAGS += -c -I$(DEPLOYDIR)include
ifeq ($(OS),MINGW)
  CXXFLAGS += -DSTAFF_UTILS_DLL_EXPORTS=1
endif
LDFLAGS += $(LDLIB)$(LIBTARGETVER) $(HOSTLDFLAGS)
ifeq ($(LINUX_RELEASE),MCBC)
  PTHREAD_VERSION := $(shell readlink /lib/libpthread.so.0 | sed 's/.*-\([\.0-9]*\)\..*/\1/')
  PTHREAD_VERSION_MAJOR := $(shell echo $(PTHREAD_VERSION) | cut -d'.' -f1)
  PTHREAD_VERSION_MINOR := $(shell echo $(PTHREAD_VERSION) | cut -d'.' -f2)
  CXXFLAGS += -DPTHREAD_VERSION_MAJOR=$(PTHREAD_VERSION_MAJOR) -DPTHREAD_VERSION_MINOR=$(PTHREAD_VERSION_MINOR)
endif

# detect iconv usability
ICONV_INCLUDE="\#include <iconv.h>"
WITH_ICONV := $(shell echo $(ICONV_INCLUDE) | LANG=C `$(CC) -print-prog-name=cc1plus` $(filter -I%,$(CXXFLAGS)) >/dev/null 2>&1 && echo "-DWITH_ICONV"; rm -f gccdump.s)
CXXFLAGS += $(WITH_ICONV)
ifneq ($(WITH_ICONV),)
ifeq ($(OS),FreeBSD)
LDFLAGS += -liconv
endif
ifeq ($(OS),Darwin)
LDFLAGS += -liconv
endif
endif

ifeq ($(OS),FreeBSD)
LDFLAGS += -lcompat
endif

ifeq ($(OS),MINGW)
LDFLAGS += -limagehlp
endif

ifeq ($(OS),SunOS)
SUNOS_VERSION := $(shell uname -v)
CXXFLAGS += -DSUNOS_MAJOR=$(subst ., -DSUNOS_MINOR=,$(SUNOS_VERSION))
endif


ifneq ($(OS),SunOS)
ifneq ($(OS),FreeBSD)
ifneq ($(OS),MINGW)
  LDFLAGS += -ldl
ifneq ($(OS),Darwin)
# detect bfd usability
BFD_INCLUDE="\#include <bfd.h>"
WITH_BFD := $(shell echo $(BFD_INCLUDE) | LANG=C `$(CC) -print-prog-name=cc1plus` $(filter -I%,$(CXXFLAGS)) >/dev/null 2>&1 && echo "-DWITH_BFD"; rm -f gccdump.s)
CXXFLAGS += $(WITH_BFD)
ifneq ($(WITH_BFD),)
  LDFLAGS += -lbfd -liberty
endif
endif
endif
endif
endif

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

HEADERS := $(wildcard $(patsubst %,%*.h*,$(SRCDIR)))
SOURCES := $(wildcard $(patsubst %,%*.cpp,$(SRCDIR)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(LIBTARGETVER) deploy

# link
$(OUTDIR)$(LIBTARGETVER): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(LIBTARGETVER) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# == deploy ========================================
deploy: "$(DEPLOYDIR)" deploy_libs deploy_headers

deploy_libs: "$(DEPLOYDIR)$(LIBDIR)" $(OUTDIR)$(LIBTARGETVER)
	cp -f $(OUTDIR)$(LIBTARGETVER) $(DEPLOYDIR)$(LIBDIR)$(LIBTARGETVER)
	$(LN) $(LIBTARGETVER) $(DEPLOYDIR)$(LIBDIR)$(LIBTARGET)

deploy_headers: "$(DEPLOYDIR)$(INCDIR)"
	cp -f $(HEADERS) $(DEPLOYDIR)$(INCDIR)
	find  $(DEPLOYDIR)$(INCDIR) -type d | xargs chmod a+x
	$(LN) staff-$(VERSION) $(DEPLOYDIR)$(INCDIR_STAFF)

# == distrib =========================================
distrib:;

# == install ========================================
install: check install_libs install_headers

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
