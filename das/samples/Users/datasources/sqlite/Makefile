include ../../../../Makefile.env

NAMESPACE = staff.das.samples

# == make ===========================================
make: check datasources wsdl deploy

datasources: "out/"
	STAFF_HOME_QUOTED=$$(echo '$(STAFF_HOME)' | sed 's/\//\\\//g');\
	for ds in $(patsubst datasources/%,%,$(wildcard datasources/*.datasources));\
do\
  sed 's/$$(STAFF_HOME)/'"$$STAFF_HOME_QUOTED"'/gm' datasources/$$ds > out/$$ds;\
done;\
	cp -f datasources/*.datatypes out/

wsdl: "out/"
	staff_codegen -u -twsdl -pdasdatasource $(wildcard datasources/*.datasources) -oout

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(COMDIR)staff.das/datasources/$(NAMESPACE)/"
	cp -f out/*.datasources out/*.datatypes $(DEPLOYDIR)$(COMDIR)staff.das/datasources/$(NAMESPACE)
	for wsdl in $(patsubst out/%,%,$(wildcard out/*.wsdl));\
do\
 destdir="$(DEPLOYDIR)$(COMDIR)$$(echo $$wsdl | sed 's/\.*[^\.]*\.wsdl$$//g')/";\
 mkdir -p $$destdir && cp -f out/$$wsdl $$destdir;\
done

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(COMDIR)staff.das/datasources/$(NAMESPACE)/"
	cp -f out/*.datasources out/*.datatypes $(INSTALLDIR)$(COMDIR)staff.das/datasources/$(NAMESPACE)/
	for wsdl in $(patsubst out/%,%,$(wildcard out/*.wsdl));\
do\
 destdir="$(INSTALLDIR)$(COMDIR)$$(echo $$wsdl | sed 's/\.*[^\.]*\.wsdl$$//g')/";\
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
