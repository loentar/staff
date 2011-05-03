TARGET = rise
PROJECT = $(TARGET)
VERSION = 0.2.8

TOPDIR ?= $(shell pwd | sed 's/$$/\//g' | sed 's/^\(.*\/rise\/\).*/\1/g' | grep \/rise\/)
include $(TOPDIR)/../Makefile.env

SRCDIR = rise/
TARGETLIB = $(LIBPREFIX)$(TARGET)$(LIBEXT)
TARGETLIBVER = $(LIBPREFIX)$(TARGET)$(LIBVEREXT)


RISE_LOG_SRC_RECODE=\"utf-8\"

ifeq ($(shell test "$(OS)" = "FreeBSD" -o "$(LINUX_RELEASE)" = "MCBC" && echo x),x)
  RISE_LOG_DST_RECODE=\"koi8-r\"
else
  RISE_LOG_DST_RECODE=\"utf-8\"
endif

ifeq ($(LINUX_RELEASE),MCBC)
  PTHREAD_VERSION := $(shell readlink /lib/libpthread.so.0 | sed 's/.*-\([\.0-9]*\)\..*/\1/')
  PTHREAD_VERSION_MAJOR := $(shell echo $(PTHREAD_VERSION) | cut -d'.' -f1)
  PTHREAD_VERSION_MINOR := $(shell echo $(PTHREAD_VERSION) | cut -d'.' -f2)
  CXXFLAGS += -DPTHREAD_VERSION_MAJOR=$(PTHREAD_VERSION_MAJOR) -DPTHREAD_VERSION_MINOR=$(PTHREAD_VERSION_MINOR)
endif

CXXFLAGS += -DRISE_LOG_SRC_RECODE=$(RISE_LOG_SRC_RECODE) -DRISE_LOG_DST_RECODE=$(RISE_LOG_DST_RECODE)
ifeq ($(OS),MINGW)
  CXXFLAGS += -DRISE_DLL_EXPORTS=1
endif

ifneq ($(OS),MINGW)
USRINCDIR = /usr/include/
USRINCDIRVER = $(USRINCDIR)rise-$(VERSION)/
USRLIBDIR = /usr/lib/
else
USRINCDIR = $(STAFF_HOME)/include/
USRINCDIRVER = $(USRINCDIR)rise/
USRLIBDIR = $(STAFF_HOME)/lib/
endif

DEPLOYDIR = $(TOPDIR)/deploy/$(PLATFORM)/
DEPLOYINCDIR = $(DEPLOYDIR)usr/include/
ifneq ($(OS),MINGW)
DEPLOYINCDIRVER = $(DEPLOYINCDIR)rise-$(VERSION)/
else
DEPLOYINCDIRVER = $(DEPLOYINCDIR)rise/
endif
DEPLOYLIBDIR = $(DEPLOYDIR)usr/lib/

ifneq ($(OS),MINGW)
  CXXFLAGS += -fPIC -rdynamic
endif
LDFLAGS = -lpthread -lstdc++
ifneq ($(OS),FreeBSD)
ifneq ($(OS),MINGW)
  LDFLAGS += -ldl
endif
endif
ifeq ($(OS),MINGW)
  LDFLAGS += -lws2_32 -limagehlp
endif
LDFLAGS += $(LDLIB)$(TARGETLIBVER)

ifeq ($(RISE_NO_DEMANGLE),)
ifneq ($(OS),MINGW)
ifneq ($(wildcard /usr/lib/libiberty_pic.a),)
  DEMANGLELIBNAME = /usr/lib/libiberty_pic.a
  CXXFLAGS += -DRISE_USE_DEMANGLE=1
  DEMANGLEOBJDIR = $(OBJDIR)iberty/
  LDFLAGS += $(DEMANGLEOBJDIR)*.o
else
ifneq ($(wildcard /usr/lib/libiberty.a),)
  DEMANGLELIBNAME = /usr/lib/libiberty.a
  CXXFLAGS += -DRISE_USE_DEMANGLE=1
  DEMANGLEOBJDIR = $(OBJDIR)iberty/
  LDFLAGS += $(DEMANGLEOBJDIR)*.o
endif
endif
endif
endif

SRCDIRS := $(wildcard $(SRCDIR)*)
HEADERS := $(patsubst $(SRCDIR)%,%,$(wildcard $(patsubst %,%/*.h*,$(SRCDIRS))))

VPATH = $(subst $(empty) $(empty),:,$(SRCDIRS))

SOURCES := $(wildcard $(patsubst %,%/*.cpp,$(SRCDIRS)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.PHONY: test

# == make ===========================================
make: dirs $(OUTDIR)$(TARGETLIBVER) deploy

dirs: "$(OBJDIR)" "$(OUTDIR)" "$(DEPLOYLIBDIR)" "$(DEPLOYINCDIR)"

# == compile ========================================
$(OBJDIR)%.o: %.cpp
	$(CXX) -I. -c $(CXXFLAGS) $< -o $@

$(OUTDIR)$(TARGETLIBVER): $(OBJECTS)
	[ ! -z "$(DEMANGLEOBJDIR)" ] && mkdir -p $(DEMANGLEOBJDIR) && cd $(DEMANGLEOBJDIR) && ar -x $(DEMANGLELIBNAME) || true
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGETLIBVER) $(LDFLAGS)

# == deploy  ========================================
deploy: deploy_headers deploy_lib
	chmod -R go-w $(DEPLOYDIR)

deploy_lib: "$(DEPLOYLIBDIR)" $(OUTDIR)$(TARGETLIBVER)
	cp -f $(OUTDIR)$(TARGETLIBVER) $(DEPLOYLIBDIR)$(TARGETLIBVER)
ifneq ($(OS),MINGW)
	ln -sf $(TARGETLIBVER) $(DEPLOYLIBDIR)$(TARGETLIB)
endif

deploy_headers: "$(DEPLOYINCDIRVER)"
	@echo "deploying headers to $(DEPLOYINCDIRVER)";\
 cp -Rf $(SRCDIR)/* $(DEPLOYINCDIRVER);\
 chmod -R a+r $(DEPLOYINCDIRVER);\
 chmod -R u+w $(DEPLOYINCDIRVER);\
 find $(DEPLOYINCDIRVER) -type f -a \( \! -name '*.h' -a  \! -name '*.hpp' \) | xargs rm -f;\
 find $(DEPLOYINCDIRVER) -type d -name .svn | xargs rm -Rf;\
 find $(DEPLOYINCDIRVER) -type d | xargs chmod a+x;
ifneq ($(OS),MINGW)
	ln -nsf rise-$(VERSION) $(DEPLOYINCDIR)rise
endif

include ../Makefile.distrib

# == install ========================================
install: install_headers install_libs

install_libs: $(OUTDIR)$(TARGETLIBVER) "$(USRLIBDIR)"
	cp -f $(OUTDIR)$(TARGETLIBVER) $(USRLIBDIR)$(TARGETLIBVER)
ifneq ($(OS),MINGW)
	ln -sf $(TARGETLIBVER) $(USRLIBDIR)$(TARGETLIB)
endif
ifeq ($(OS),Linux)
	test -f /usr/lib/libbfd.so || ln -sf $$(find /usr/lib/libbfd-*.so | head -1) /usr/lib/libbfd.so
endif

install_headers: "$(USRINCDIRVER)"
	@echo "installing headers to $(USRINCDIRVER)";\
 cp -Rf $(SRCDIR)/* $(USRINCDIRVER);\
 chmod -R a+r $(USRINCDIRVER);\
 find $(USRINCDIRVER) -type f -a \( \! -name '*.h' -a  \! -name '*.hpp' \) | xargs rm -f;\
 find $(USRINCDIRVER) -type d -name .svn | xargs rm -Rf;\
 find $(USRINCDIRVER) -type d | xargs chmod a+x;
ifneq ($(OS),MINGW)
	ln -nsf rise-$(VERSION) $(USRINCDIR)rise
endif

# == uninstall ======================================
uninstall:
	rm -Rf $(USRINCDIR)rise $(USRLIBDIR)$(TARGETLIB) $(USRLIBDIR)$(TARGETLIBVER)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

test: make
	$(MAKE) -C test/rise_test test

# == mkdir ==========================================
"%/":
	@mkdir -p $@ && chmod g+w $@
