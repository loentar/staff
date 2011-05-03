include ../../Makefile.env

TARGET = axis2services
TARGETNAME = .libs/mod_$(TARGET).so

# == make ===========================================
ifneq ($(APXS),)
make: $(TARGETNAME) deploy
else
make:
	@$(ECHO) "\033[33;1m===============================================================================\033[0m"
	@$(ECHO) "\033[33;1mWARNING: Can't find apache apxs. Apache2 module \"$(TARGET)\" will not build\033[0m"
	@$(ECHO) "\033[33;1m===============================================================================\033[0m"
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


$(TARGETNAME):
	$(APXS) -c mod_$(TARGET).c

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
	ln -sf ../mods-available/$(TARGET).conf $(DEPLOYROOTDIR)etc/apache2/mods-enabled/$(TARGET).conf
	ln -sf ../mods-available/$(TARGET).load $(DEPLOYROOTDIR)etc/apache2/mods-enabled/$(TARGET).load
	cp -f $(TARGETNAME) $(DEPLOYROOTDIR)usr/lib/apache2/modules/
else
ifeq ($(APXS),/www/bin/apxs)
	mkdir -p $(DEPLOYROOTDIR)www/conf/mods-enabled/
	cat conf/$(TARGET).load conf/$(TARGET).conf | sed 's/\/usr\/lib\/apache2\//\/www\//g' > $(DEPLOYROOTDIR)www/conf/mods-enabled/$(TARGET).conf
	grep -q 'Include conf/mods-enabled/\*' /www/conf/httpd.conf || echo -e '\nInclude conf/mods-enabled/*\n\n' >> /www/conf/httpd.conf
	cp -f $(TARGETNAME) $(DEPLOYROOTDIR)www/modules/
else
	test ! -z "$(APACHECONFDIR)" -a -d "$(APACHECONFDIR)"
	test ! -z "$(APACHEMODDIR)" -a -d "$(APACHEMODDIR)"
	cat conf/$(TARGET).load conf/$(TARGET).conf > $(DEPLOYROOTDIR)$(APACHECONFDIR)/$(TARGET).conf
	cp -f $(TARGETNAME) $(DEPLOYROOTDIR)$(APACHEMODDIR)
endif
endif
endif

# == distrib =========================================
distrib:;

install:
ifneq ($(APXS),)
ifeq ($(LINUX_VERSION),debian)
	cp -f conf/$(TARGET).load conf/$(TARGET).conf /etc/apache2/mods-available/
	ln -sf ../mods-available/$(TARGET).conf /etc/apache2/mods-enabled/$(TARGET).conf
	ln -sf ../mods-available/$(TARGET).load /etc/apache2/mods-enabled/$(TARGET).load
	cp -f $(TARGETNAME) /usr/lib/apache2/modules/
else
ifeq ($(APXS),/www/bin/apxs)
	mkdir -p /www/conf/mods-enabled/
	cat conf/$(TARGET).load conf/$(TARGET).conf | sed 's/\/usr\/lib\/apache2\//\/www\//g' > /www/conf/mods-enabled/$(TARGET).conf
	grep -q 'Include conf/mods-enabled/\*' /www/conf/httpd.conf || echo -e '\nInclude conf/mods-enabled/*\n\n' >> /www/conf/httpd.conf
	cp -f $(TARGETNAME) /www/modules/
else
	test ! -z "$(APACHECONFDIR)" -a -d "$(APACHECONFDIR)"
	test ! -z "$(APACHEMODDIR)" -a -d "$(APACHEMODDIR)"
	cat conf/$(TARGET).load conf/$(TARGET).conf > $(APACHECONFDIR)/$(TARGET).conf
	cp -f $(TARGETNAME) $(APACHEMODDIR)
endif
endif
endif

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
