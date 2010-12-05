TARGET = rise
PROJECT = $(TARGET)
VERSION = 0.2.8

TOPDIR ?= $(shell pwd | sed 's/$$/\//g' | sed 's/^\(.*\/rise\/\).*/\1/g' | grep \/rise\/)
include $(TOPDIR)/../Makefile.env

SRCDIR = rise/
TARGETLIB = lib$(TARGET).so
TARGETLIBVER = $(TARGETLIB).$(VERSION)


RISE_LOG_SRC_RECODE=\"utf-8\"

ifeq ($(shell test "$(OS)" = "FreeBSD" -o "$(LINUX_RELEASE)" = "MCBC" && echo x),x)
  RISE_LOG_DST_RECODE=\"koi8-r\"
else
  RISE_LOG_DST_RECODE=\"utf-8\"
endif

CXXFLAGS += -DRISE_LOG_SRC_RECODE=$(RISE_LOG_SRC_RECODE) -DRISE_LOG_DST_RECODE=$(RISE_LOG_DST_RECODE)

USRINCDIR = /usr/include/
USRINCDIRVER = /usr/include/rise-$(VERSION)/
USRLIBDIR = /usr/lib/

DEPLOYDIR = $(TOPDIR)/deploy/$(PLATFORM)/
DEPLOYINCDIR = $(DEPLOYDIR)usr/include/
DEPLOYINCDIRVER = $(DEPLOYINCDIR)rise-$(VERSION)/
DEPLOYLIBDIR = $(DEPLOYDIR)usr/lib/

CXXFLAGS += -D_REENTRANT -Wall -rdynamic -Wno-deprecated -fexceptions -fPIC
LDFLAGS = -lpthread -lstdc++
ifneq ($(OS),FreeBSD)
  LDFLAGS += -ldl
endif

ifeq ($(RISE_NO_DEMANGLE),)
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

SRCDIRS := $(wildcard $(SRCDIR)*)
HEADERS := $(patsubst $(SRCDIR)%,%,$(wildcard $(patsubst %,%/*.h*,$(SRCDIRS))))

VPATH = $(subst $(empty) $(empty),:,$(SRCDIRS))

SOURCES := $(wildcard $(patsubst %,%/*.cpp,$(SRCDIRS)))
OBJECTS := $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: dirs $(OUTDIR)$(TARGETLIBVER) deploy

dirs: "$(OBJDIR)" "$(OUTDIR)" "$(DEPLOYLIBDIR)" "$(DEPLOYINCDIR)"

# == compile ========================================
$(OBJDIR)%.o: %.cpp
	$(CXX) -I. -c $(CXXFLAGS) $< -o $@

$(OUTDIR)$(TARGETLIBVER): $(OBJECTS)
	[ ! -z "$(DEMANGLEOBJDIR)" ] && mkdir -p $(DEMANGLEOBJDIR) && cd $(DEMANGLEOBJDIR) && ar -x $(DEMANGLELIBNAME) || true
	$(CXX) -shared -Wl,-soname,$(TARGETLIBVER) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETLIBVER)

# == deploy  ========================================
deploy: deploy_headers deploy_lib
	chmod -R go-w $(DEPLOYDIR)

deploy_lib: "$(DEPLOYLIBDIR)" $(OUTDIR)$(TARGETLIBVER)
	cp -f $(OUTDIR)$(TARGETLIBVER) $(DEPLOYLIBDIR)$(TARGETLIBVER)
	ln -sf $(TARGETLIBVER) $(DEPLOYLIBDIR)$(TARGETLIB)

deploy_headers: "$(DEPLOYINCDIRVER)"
	@echo "deploying headers to $(DEPLOYINCDIRVER)";\
 cp -Rf $(SRCDIR) $(DEPLOYINCDIRVER);\
 chmod -R a+r $(DEPLOYINCDIRVER);\
 chmod -R u+w $(DEPLOYINCDIRVER);\
 find $(DEPLOYINCDIRVER) -type d -name .svn | xargs rm -Rf;\
 find $(DEPLOYINCDIRVER) -type d | xargs chmod a+x;\
 ln -nsf rise-$(VERSION) $(DEPLOYINCDIR)rise

include ../Makefile.distrib

# == install ========================================
install: install_headers install_libs

install_libs: $(OUTDIR)$(TARGETLIBVER)
	cp -f $(OUTDIR)$(TARGETLIBVER) $(USRLIBDIR)$(TARGETLIBVER)
	ln -sf $(TARGETLIBVER) $(USRLIBDIR)$(TARGETLIB)
ifeq ($(OS),Linux)
	test -f /usr/lib/libbfd.so || ln -sf $$(find /usr/lib/libbfd-*.so | head -1) /usr/lib/libbfd.so
endif

install_headers: "$(USRINCDIRVER)"
	@echo "installing headers to $(USRINCDIRVER)";\
 cp -Rf $(SRCDIR) $(USRINCDIRVER);\
 chmod -R a+r $(USRINCDIRVER);\
 find $(USRINCDIRVER) -type d -name .svn | xargs rm -Rf;\
 find $(USRINCDIRVER) -type d | xargs chmod a+x;\
 ln -nsf rise-$(VERSION) $(USRINCDIR)rise

# == uninstall ======================================
uninstall:
	rm -Rf $(USRINCDIR)rise $(USRLIBDIR)$(TARGETLIB) $(USRLIBDIR)$(TARGETLIBVER)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@mkdir -p $@ && chmod g+w $@
