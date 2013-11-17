include ../Makefile.env
include ../VERSION

ifneq "$(wildcard /etc/debian_version)" ""
  PKG = deb
endif

ifeq ($(AXIS2C_HOME),)
AXIS2C_LIBDIR := $(shell pkg-config --libs-only-L axis2c | sed 's;^-L;;;s;/$$;;')
ifneq ($(wildcard $(dir $(AXIS2C_LIBDIR))modules),)
AXIS2C_HOME := $(dir $(AXIS2C_LIBDIR))
endif
else
AXIS2C_LIBDIR = $(AXIS2C_HOME)/lib
endif

DEPLOYDIR := $(TOPDIR)deploy/$(PLATFORM)/

DISTRIBDIR_DEPLOY_LIBS = $(prefix)/staff-libs/
DISTRIBDIR_DEPLOY_DEV = $(prefix)/staff-dev/
DISTRIBDIR_DEPLOY_MOD = $(prefix)/libapache2-mod-staff-axis2services/

# == distrib ========================================
distrib:
ifneq ($(PKG),)
	$(MAKE) -C$(PKG) $(MAKECMDGOALS)
else
	@echo "Unsupported build system"; false
endif

# == distrib_deploy =================================
distrib_deploy: distrib_deploy_check distrib_deploy_libs distrib_deploy_dev distrib_deploy_apache2_mod

distrib_deploy_check:
	@test -d $(DEPLOYDIR) || (echo "ERROR: Project was'nt built. Type 'make' to build it." >&2; false)
	@test -n "$(prefix)" || (echo "ERROR: prefix is empty." >&2; false)
	@echo Deploying build to $(prefix)

distrib_deploy_libs:
	@mkdir -p $(DISTRIBDIR_DEPLOY_LIBS)usr/lib/ $(DISTRIBDIR_DEPLOY_LIBS)usr/lib/staff/db $(DISTRIBDIR_DEPLOY_LIBS)usr/lib/staff/components
	@find $(DEPLOYDIR)$(STAFF_HOME)/lib/ -maxdepth 1 -type f -a \! -name '*codegen*' -exec cp -dp {} $(DISTRIBDIR_DEPLOY_LIBS)usr/lib/ \;
	@cp -Rdpf $(DEPLOYDIR)$(STAFF_HOME)/components/* $(DISTRIBDIR_DEPLOY_LIBS)usr/lib/staff/components/
	@cp -Rdpf $(DEPLOYDIR)$(STAFF_HOME)/db/* $(DISTRIBDIR_DEPLOY_LIBS)usr/lib/staff/db/
	@# ------ axis2/c modules and services
	@mkdir -p $(DISTRIBDIR_DEPLOY_LIBS)usr/lib/axis2/
	@cp -Rdpf $(DEPLOYDIR)$(AXIS2C_HOME)* $(DISTRIBDIR_DEPLOY_LIBS)usr/lib/axis2/
	@# ---- man documentation ----
	@mkdir -p $(DISTRIBDIR_DEPLOY_LIBS)usr/share/man/man1 && cp -Rf $(TOPDIR)/distrib/man/staff.1 $(DISTRIBDIR_DEPLOY_LIBS)usr/share/man/man1/

distrib_deploy_dev:
	@mkdir -p $(DISTRIBDIR_DEPLOY_DEV)usr/bin/ $(DISTRIBDIR_DEPLOY_DEV)usr/include/ $(DISTRIBDIR_DEPLOY_DEV)usr/lib/staff/codegen
	@cp -Rpdf $(DEPLOYDIR)$(STAFF_HOME)/include/* $(DISTRIBDIR_DEPLOY_DEV)usr/include/
	@cp -Rdpf $(DEPLOYDIR)$(STAFF_HOME)/bin/staff_codegen $(DISTRIBDIR_DEPLOY_DEV)usr/bin/
	@find $(DEPLOYDIR)$(STAFF_HOME)/lib/ -maxdepth 1 -type l -a \! -name '*codegen*' -exec cp -dp {} $(DISTRIBDIR_DEPLOY_DEV)usr/lib/ \;
	@cp -Rdpf $(DEPLOYDIR)$(STAFF_HOME)/lib/*codegenparser* $(DISTRIBDIR_DEPLOY_DEV)usr/lib/
	@cp -Rdpf $(DEPLOYDIR)$(STAFF_HOME)/lib/codegen/* $(DISTRIBDIR_DEPLOY_DEV)usr/lib/staff/codegen/
	@mkdir -p $(DISTRIBDIR_DEPLOY_DEV)/usr/lib/pkgconfig/
	@sed "s:__STAFF_HOME__:$(STAFF_HOME):g;s/__VERSION__/$(PACKAGE_VERSION)/g" $(TOPDIR)/distrib/pkgconfig/wsf-staff.pc > $(DISTRIBDIR_DEPLOY_DEV)/usr/lib/pkgconfig/wsf-staff.pc
	@# ---- man documentation ----
	@mkdir -p $(DISTRIBDIR_DEPLOY_DEV)usr/share/man/man1 && cp -Rf $(TOPDIR)/distrib/man/staff_codegen.1 $(DISTRIBDIR_DEPLOY_DEV)usr/share/man/man1/

distrib_deploy_apache2_mod:
	cp -Rdpf $(DEPLOYDIR)etc $(DISTRIBDIR_DEPLOY_MOD)
	mkdir -p $(DISTRIBDIR_DEPLOY_MOD)usr/lib/apache2/modules/
	cp -Rdpf $(DEPLOYDIR)usr/lib/apache2/modules/* $(DISTRIBDIR_DEPLOY_MOD)usr/lib/apache2/modules/

# == distclean ======================================
distclean:
ifneq ($(PKG),)
	$(MAKE) -C$(PKG) $(MAKECMDGOALS)
endif


# == mkdir ==========================================
"%/":
	@mkdir -p $@ && chmod g+w $@
