include ../../Makefile.env

# == make ===========================================
make: check deploy

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(LIBDIR)codegen/"
	cp -Rf templates $(DEPLOYDIR)$(LIBDIR)codegen/
	find $(DEPLOYDIR)$(LIBDIR)codegen/ -type d -name '.svn' | xargs rm -Rf

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(LIBDIR)codegen/"
	cp -Rf templates $(INSTALLDIR)$(LIBDIR)codegen/
	find $(INSTALLDIR)$(LIBDIR)codegen/templates -type d -name '.svn' | xargs rm -Rf

# == uninstall ======================================
uninstall: check

# == clean ==========================================
clean:

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
