include ../../Makefile.env

TARGET = axis2services
TARGETNAME = .libs/mod_$(TARGET).so

# == make ===========================================
ifeq ($(CROSSCOMPILE),0)
ifneq ($(APXS),)
make: $(TARGETNAME) deploy
else
make:
	@$(ECHO) "\n\033[33;1mSkipping Apache2 module \"$(TARGET)\" compilation: Apache apxs is not found.\033[0m\n"
endif
else
make:
	@$(ECHO) "Crosscompilation of axis2services is not supported now"
endif

ifeq ($(OS),FreeBSD)
APACHECONFDIR=/usr/local/etc/apache22
APACHEMODDIR=/usr/local/libexec/apache22
else
ifeq ($(OS),Darwin)
APACHECONFDIR=/etc/apache2
APACHEMODDIR=/usr/libexec/apache2
endif
endif

APXS_CFLAGS = $(shell $(APXS) -q CFLAGS)
ifneq ($(DEBUG),)
  APXS_CFLAGS += -D_DEBUG -g -O0
endif

$(TARGETNAME): mod_$(TARGET).c
	$(APXS) -S CFLAGS="$(APXS_CFLAGS)" -c mod_$(TARGET).c

# == clean ==========================================
clean:
	rm -f mod_$(TARGET).o mod_$(TARGET).lo mod_$(TARGET).slo mod_$(TARGET).la 
	rm -Rf .libs

# == deploy ==========================================
deploy: $(TARGETNAME)
ifneq ($(APXS),)
ifeq ($(LINUX_VERSION),debian)
	mkdir -p $(DEPLOYROOTDIR)etc/apache2/mods-available/ $(DEPLOYROOTDIR)etc/apache2/mods-enabled/ $(DEPLOYROOTDIR)usr/lib/apache2/modules/
	cp -f conf/$(TARGET).load conf/$(TARGET).conf $(DEPLOYROOTDIR)etc/apache2/mods-available/
	$(LN) ../mods-available/$(TARGET).conf $(DEPLOYROOTDIR)etc/apache2/mods-enabled/$(TARGET).conf
	$(LN) ../mods-available/$(TARGET).load $(DEPLOYROOTDIR)etc/apache2/mods-enabled/$(TARGET).load
	cp -f $(TARGETNAME) $(DEPLOYROOTDIR)usr/lib/apache2/modules/
else
ifeq ($(APXS),/www/bin/apxs)
	mkdir -p $(DEPLOYROOTDIR)etc/apache2/mods-available/ $(DEPLOYROOTDIR)www/conf/mods-enabled/ $(DEPLOYROOTDIR)www/modules/
	cat conf/$(TARGET).load conf/$(TARGET).conf | sed 's/\/usr\/lib\/apache2\//\/www\//g' > $(DEPLOYROOTDIR)www/conf/mods-enabled/$(TARGET).conf
	grep -q 'Include conf/mods-enabled/\*' /www/conf/httpd.conf || echo -e '\nInclude conf/mods-enabled/*\n\n' >> /www/conf/httpd.conf
	cp -f $(TARGETNAME) $(DEPLOYROOTDIR)www/modules/
else
	test ! -z "$(APACHECONFDIR)" -a -d "$(APACHECONFDIR)"
	test ! -z "$(APACHEMODDIR)" -a -d "$(APACHEMODDIR)"
	mkdir -p $(DEPLOYROOTDIR)$(APACHECONFDIR) $(DEPLOYROOTDIR)$(APACHEMODDIR)
	cat conf/$(TARGET).load conf/$(TARGET).conf > $(DEPLOYROOTDIR)$(APACHECONFDIR)/$(TARGET).conf
	cp -f $(TARGETNAME) $(DEPLOYROOTDIR)$(APACHEMODDIR)
endif
endif
endif

# == distrib =========================================
distrib:;

ifeq ($(CROSSCOMPILE),0)
install:
ifneq ($(APXS),)
ifeq ($(LINUX_VERSION),debian)
	test -e /etc/apache2/mods-available/$(TARGET).conf || cp -f conf/$(TARGET).conf /etc/apache2/mods-available/
	test -e /etc/apache2/mods-available/$(TARGET).load || cp -f conf/$(TARGET).load /etc/apache2/mods-available/
	$(LN) ../mods-available/$(TARGET).conf /etc/apache2/mods-enabled/$(TARGET).conf
	$(LN) ../mods-available/$(TARGET).load /etc/apache2/mods-enabled/$(TARGET).load
	cp -f $(TARGETNAME) /usr/lib/apache2/modules/
	-/etc/init.d/apache2 status >/dev/null 2>&1 && (\
 /etc/init.d/apache2 stop;\
 test -z "$(DEBUG)" || rm -f /tmp/axis2services_debug_*.log;\
  /etc/init.d/apache2 start) &
else
ifeq ($(APXS),/www/bin/apxs)
	mkdir -p /www/conf/mods-enabled/
	test -e /www/conf/mods-enabled/$(TARGET).conf || cat conf/$(TARGET).load conf/$(TARGET).conf | sed 's/\/usr\/lib\/apache2\//\/www\//g' > /www/conf/mods-enabled/$(TARGET).conf
	grep -q 'Include conf/mods-enabled/\*' /www/conf/httpd.conf || echo -e '\nInclude conf/mods-enabled/*\n\n' >> /www/conf/httpd.conf
	cp -f $(TARGETNAME) /www/modules/
else
	test ! -z "$(APACHECONFDIR)" -a -d "$(APACHECONFDIR)"
	test ! -z "$(APACHEMODDIR)" -a -d "$(APACHEMODDIR)"
	test -e $(APACHECONFDIR)/$(TARGET).conf || cat conf/$(TARGET).load conf/$(TARGET).conf > $(APACHECONFDIR)/$(TARGET).conf
	cp -f $(TARGETNAME) $(APACHEMODDIR)
endif
endif
endif
else
install:;
endif

ifeq ($(CROSSCOMPILE),0)
uninstall:
ifneq ($(APXS),)
ifeq ($(LINUX_VERSION),debian)
	rm -f ../mods-available/$(TARGET).conf /etc/apache2/mods-enabled/$(TARGET).conf
	rm -f ../mods-available/$(TARGET).load /etc/apache2/mods-enabled/$(TARGET).load
	rm -f /usr/lib/apache2/modules/$(TARGETNAME)
	test -z "$(DEBUG)" || rm -f /tmp/axis2services_debug_*.log
	-/etc/init.d/apache2 status >/dev/null 2>&1 && /etc/init.d/apache2 restart &
else
ifeq ($(APXS),/www/bin/apxs)
	rm -f /www/conf/mods-enabled/$(TARGET).conf
	rm -f /www/modules/$(TARGETNAME)
else
	test ! -z "$(APACHECONFDIR)" -a -d "$(APACHECONFDIR)"
	test ! -z "$(APACHEMODDIR)" -a -d "$(APACHEMODDIR)"
	rm -f $(APACHECONFDIR)/$(TARGET).conf
	rm -f $(APACHEMODDIR)/$(TARGETNAME)
endif
endif
endif
uninstall:
endif


# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
