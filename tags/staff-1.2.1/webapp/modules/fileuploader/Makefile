include ../../Makefile.env

#builddir = .
#top_srcdir = /usr/
#top_builddir = /usr/
#include $(top_srcdir)build/special.mk

ifeq ($(OS),UBUNTU)
APXS = apxs2
else
ifeq ($(OS),MCBC)
APXS = /www/bin/apxs
else
APXS = $(shell which apxs || which apxs2)
endif
endif

TARGET = fileuploader
TARGETNAME = .libs/mod_$(TARGET).so

# == make ===========================================
make: $(TARGETNAME) deploy

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
ifeq ($(OS),UBUNTU)
	cp -f conf/$(TARGET).load conf/$(TARGET).conf /etc/apache2/mods-available/
	ln -sf ../mods-available/$(TARGET).conf /etc/apache2/mods-enabled/$(TARGET).conf
	ln -sf ../mods-available/$(TARGET).load /etc/apache2/mods-enabled/$(TARGET).load
	cp -f $(TARGETNAME) /usr/lib/apache2/modules/
else
ifeq ($(OS),MCBC)
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

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
