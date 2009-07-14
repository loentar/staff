
TARGET = rise
PROJECT = $(TARGET)
VERSION = 0.2.8

SRCDIR = rise/
TARGETLIB = lib$(TARGET).so
TARGETLIBVER = $(TARGETLIB).$(VERSION)

ifeq ("$(PLATFORM)","")
  PLATFORM := $(shell uname -m)
endif

ifeq ($(TOPDIR),)
  TOPDIR = $(shell pwd | sed 's/$$/\//g' | sed 's/^\(.*\/rise\/\).*/\1/g' | grep \/rise\/)
endif

ifneq ($(shell grep -c vniins /proc/version),0)
  OS = MCBC
  RISE_LOG_SRC_RECODE=\"utf-8\"
  RISE_LOG_DST_RECODE=\"koi8-r\"
else
ifneq ($(shell grep -c ubuntu /proc/version),0)
  OS = UBUNTU
  RISE_LOG_SRC_RECODE=\"utf-8\"
  RISE_LOG_DST_RECODE=\"utf-8\"
else
  OS = UNKNOWN
  RISE_LOG_SRC_RECODE=\"utf-8\"
  RISE_LOG_DST_RECODE=\"utf-8\"
endif
endif

CXXFLAGS += -DOS_$(OS) -DRISE_LOG_SRC_RECODE=$(RISE_LOG_SRC_RECODE) -DRISE_LOG_DST_RECODE=$(RISE_LOG_DST_RECODE)

USRINCDIR = /usr/include/
USRINCDIRVER = /usr/include/rise-$(VERSION)/
USRLIBDIR = /usr/lib/

OBJDIR = obj/$(PLATFORM)/
OUTDIR = out/$(PLATFORM)/

DEPLOYDIR = $(TOPDIR)/deploy/$(PLATFORM)/
DEPLOYINCDIR = $(DEPLOYDIR)usr/include/
DEPLOYINCDIRVER = $(DEPLOYINCDIR)rise-$(VERSION)/
DEPLOYLIBDIR = $(DEPLOYDIR)usr/lib/

#DISTRIBDIR = $(TOPDIR)/../distrib/

CXXFLAGS += -D_REENTRANT -Wall -rdynamic -Wno-deprecated -fexceptions -fPIC
LIBS = -ldl -lpthread -lstdc++

ifeq ($(PLATFORM),x86_64)
  CXXFLAGS += -D_M_X64
endif

ifndef ("$(DEBUG)","")
  CXXFLAGS += -DDEBUG -g -O0
  LDFLAGS += -g -O0
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
	$(CXX) -shared -Wl,-soname,$(TARGETLIBVER) $(LIBS) $(CXXFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETLIBVER)

# == deploy  ========================================
deploy: deploy_headers deploy_lib
	chmod -R go-w $(DEPLOYDIR)

deploy_lib: "$(DEPLOYLIBDIR)" $(OUTDIR)$(TARGETLIBVER)
	cp -f $(OUTDIR)$(TARGETLIBVER) $(DEPLOYLIBDIR)$(TARGETLIBVER)
	ln -sf $(TARGETLIBVER) $(DEPLOYLIBDIR)$(TARGETLIB)

deploy_headers: "$(DEPLOYINCDIRVER)"
	@echo "deploying headers to $(DEPLOYINCDIRVER)";\
 cd $(SRCDIR);\
 cp --parents --force $(HEADERS) $(DEPLOYINCDIRVER);\
 chmod -R a+r $(DEPLOYINCDIRVER);\
 find  $(DEPLOYINCDIRVER) -type d | xargs chmod a+x;\
 ln -nsf rise-$(VERSION) $(DEPLOYINCDIR)rise

include ../Makefile.distrib

# == distrib  ========================================
#distrib:
#	make -C distrib distrib
# "$(DISTRIBDIR)" $(patsubst $(DISTRIBSCRIPTSDIR)%,%,$(wildcard $(DISTRIBSCRIPTSDIR)*))

#%.spec:
#	@/bin/echo -e "\n\n\033[1mbuilding rise-$(VERSION)-$(patsubst %.spec,%,$@).$(PLATFORM).rpm\033[0m\n"
#	sed "s/__version__/$(VERSION)/g" $(DISTRIBSCRIPTSDIR)$@ >/tmp/buildrpm_$@; \
# rpmbuild -bb /tmp/buildrpm_$@ --buildroot $(DEPLOYDIR) --target $(PLATFORM) --quiet; \
# mv /usr/src/RPM/RPMS/$(PLATFORM)/rise-$(VERSION)-$(patsubst %.spec,%,$@).$(PLATFORM).rpm $(DISTRIBDIR); \
# rm -f /tmp/buildrpm_$@.spec


# == install ========================================
install: install_headers install_libs

install_libs: $(OUTDIR)$(TARGETLIBVER)
	cp -f $(OUTDIR)$(TARGETLIBVER) $(USRLIBDIR)$(TARGETLIBVER)
	ln -sf $(TARGETLIBVER) $(USRLIBDIR)$(TARGETLIB)

install_headers: "$(USRINCDIRVER)"
	@echo "installing headers to $(USRINCDIRVER)";\
 cd $(SRCDIR);\
 cp --parents --force $(HEADERS) $(USRINCDIRVER);\
 chmod -R a+r $(USRINCDIRVER);\
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
