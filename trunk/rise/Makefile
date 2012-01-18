TARGET = rise
PROJECT = $(TARGET)

TOPDIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST)))/..)/
ifeq ($(MAKEFILE_ENV_INCLUDED),)
include $(TOPDIR)Makefile.env
endif

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
else
# detect bfd usability
  USE_BFD := $(shell echo "\#include <bfd.h>" | LANG=C `gcc -print-prog-name=cc1plus` -E >/dev/null 2>&1 && echo "-DRISE_USE_BFD")
  CXXFLAGS += $(USE_BFD)
endif

ifneq ($(OS),MINGW)
  DEPLOYINCDIRVER = $(DEPLOYINCDIR)rise-$(VERSION)/
  INSTALLINCDIRVER = $(INSTALLINCDIR)rise-$(VERSION)/
else
  DEPLOYINCDIRVER = $(DEPLOYINCDIR)rise/
  INSTALLINCDIRVER = $(INSTALLINCDIR)rise/
endif

LDFLAGS = -lpthread -lstdc++
ifneq ($(OS),FreeBSD)
ifneq ($(OS),MINGW)
  LDFLAGS += -ldl
endif
endif
ifeq ($(OS),MINGW)
  LDFLAGS += -lws2_32 -limagehlp
else
  CXXFLAGS += -rdynamic
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
# detect library usability
ifeq ($(shell l=/usr/lib/libiberty.a; o=$$(ar t $$l | head -1); ar x $$l $$o && gcc -shared -fPIC $$o -ot 2>&1; echo $?; rm -f t $$o),0)
  DEMANGLELIBNAME = /usr/lib/libiberty.a
  CXXFLAGS += -DRISE_USE_DEMANGLE=1
  DEMANGLEOBJDIR = $(OBJDIR)iberty/
  LDFLAGS += $(DEMANGLEOBJDIR)*.o
endif
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
	$(LN) $(TARGETLIBVER) $(DEPLOYLIBDIR)$(TARGETLIB)

deploy_headers: "$(DEPLOYINCDIRVER)"
	@echo "deploying headers to $(DEPLOYINCDIRVER)";\
 cp -Rf $(SRCDIR)/* $(DEPLOYINCDIRVER);\
 chmod -R a+r $(DEPLOYINCDIRVER);\
 chmod -R u+w $(DEPLOYINCDIRVER);\
 find $(DEPLOYINCDIRVER) -type f -a \( \! -name '*.h' -a  \! -name '*.hpp' \) | xargs rm -f;\
 find $(DEPLOYINCDIRVER) -type d -name .svn | xargs rm -Rf;\
 find $(DEPLOYINCDIRVER) -type d | xargs chmod a+x;
	$(LN) rise-$(VERSION) $(DEPLOYINCDIR)rise

# == install ========================================
install: install_headers install_libs
	-@[ ! -L /usr/lib/librise.so -a ! -L /usr/include/rise ] || (echo uninstalling librise from /usr/lib; rm -Rf /usr/lib/librise.so* /usr/include/rise*)

install_libs: $(OUTDIR)$(TARGETLIBVER) "$(INSTALLLIBDIR)"
	cp -f $(OUTDIR)$(TARGETLIBVER) $(INSTALLLIBDIR)$(TARGETLIBVER)
	$(LN) $(TARGETLIBVER) $(INSTALLLIBDIR)$(TARGETLIB)
ifeq ($(OS),Linux)
	test -e /usr/lib/libbfd.so || ln -nsf $$(find /usr/lib/libbfd-*.so | head -1) /usr/lib/libbfd.so
endif

install_headers: "$(INSTALLINCDIRVER)"
	@echo "installing headers to $(INSTALLINCDIRVER)";\
 cp -Rf $(SRCDIR)/* $(INSTALLINCDIRVER);\
 chmod -R a+r $(INSTALLINCDIRVER);\
 find $(INSTALLINCDIRVER) -type f -a \( \! -name '*.h' -a  \! -name '*.hpp' \) | xargs rm -f;\
 find $(INSTALLINCDIRVER) -type d -name .svn | xargs rm -Rf;\
 find $(INSTALLINCDIRVER) -type d | xargs chmod a+x;
	$(LN) rise-$(VERSION) $(INSTALLINCDIR)rise

# == uninstall ======================================
uninstall:
	rm -Rf $(INSTALLINCDIR)rise $(INSTALLINCDIRVER) $(INSTALLLIBDIR)$(TARGETLIB) $(INSTALLLIBDIR)$(TARGETLIBVER)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

test: make
	$(MAKE) -C test/rise_test test

# == mkdir ==========================================
"%/":
	@mkdir -p $@ && chmod g+w $@
