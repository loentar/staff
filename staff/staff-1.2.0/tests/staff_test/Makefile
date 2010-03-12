include ../../Makefile.env

TARGET = staff_test

# == make ===========================================
make: check deploy

# == commit =========================================
deploy: "$(DEPLOYDIR)$(BINDIR)"
	cp -f scripts/$(TARGET) $^

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(BINDIR)"
	cp -f scripts/$(TARGET) $(INSTALLDIR)$(BINDIR)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLBINDIR)$(TARGET)

# == clean ==========================================
clean:;

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
