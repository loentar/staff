include ../../../../Makefile.env

NAMESPACE = staff.das.samples

# == make ===========================================
make: wsdl check deploy

wsdl: "out/"
	staff_codegen -u -twsdl -pdasdatasource $(wildcard datasources/*.datasources) -oout

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(COMDIR)staff.das/datasources/$(NAMESPACE)/"
	cp -f datasources/* $(DEPLOYDIR)$(COMDIR)staff.das/datasources/$(NAMESPACE)
	for wsdl in $(patsubst out/%,%,$(wildcard out/*.wsdl out/*.xsd));\
do\
 destdir="$(DEPLOYDIR)$(COMDIR)$$(echo $$wsdl | sed 's/\.*[^\.]*\.\(wsdl\|\xsd\)$$//g')/";\
 mkdir -p $$destdir && cp -f out/$$wsdl $$destdir;\
done

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(COMDIR)staff.das/datasources/$(NAMESPACE)/"
	cp -f datasources/* $(INSTALLDIR)$(COMDIR)staff.das/datasources/$(NAMESPACE)/
	for wsdl in $(patsubst out/%,%,$(wildcard out/*.wsdl out/*.xsd));\
do\
 destdir="$(INSTALLDIR)$(COMDIR)$$(echo $$wsdl | sed 's/\.*[^\.]*\.\(wsdl\|\xsd\)$$//g')/";\
 mkdir -p $$destdir && cp -f out/$$wsdl $$destdir;\
done

# == uninstall ======================================
uninstall: check

# == clean ==========================================
clean:
	rm -Rf out/

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
