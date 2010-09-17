include ../../Makefile.env

TARGET = fileuploader
TARGETNAME = .libs/mod_$(TARGET).so

# == make ===========================================
ifneq ($(APXS),)
make: $(TARGETNAME) deploy
else
make:
	@/bin/echo -e "\033[33;1m===============================================================================\033[0m"
	@/bin/echo -e "\033[33;1mWARNING: Can't find apache apxs. Apache2 module \"$(TARGET)\" will not build\033[0m"
	@/bin/echo -e "\033[33;1m===============================================================================\033[0m"
endif

$(TARGETNAME):
	$(APXS) -c mod_$(TARGET).c

# == clean ==========================================
clean:
	rm -f mod_$(TARGET).o mod_$(TARGET).lo mod_$(TARGET).slo mod_$(TARGET).la 
	rm -Rf .libs

# == deploy ==========================================
deploy: $(TARGETNAME)
	mkdir -p $(DEPLOYDIR)www/conf/mods-enabled/ $(DEPLOYDIR)www/modules/
	cat conf/$(TARGET).load conf/$(TARGET).conf | sed 's/\/usr\/lib\/apache2\//\/www\//g' > $(DEPLOYDIR)/www/conf/mods-enabled/$(TARGET).conf
#	grep -q 'Include conf/mods-enabled/\*' /www/conf/httpd.conf || echo -e '\nInclude conf/mods-enabled/*\n\n' >> /www/conf/httpd.conf
	cp -f $(TARGETNAME) $(DEPLOYDIR)www/modules/

# == distrib =========================================
distrib:;

install:
ifneq ($(APXS),)
ifeq ($(OS),UBUNTU)
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
	test ! -z "$(APACHECONFDIR)"
	cat conf/$(TARGET).load conf/$(TARGET).conf > $(APACHECONFDIR)/$(TARGET).conf
	test ! -z "$(APACHEMODDIR)"
	cp -f $(TARGETNAME) $(APACHEMODDIR)
endif
endif
endif

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
