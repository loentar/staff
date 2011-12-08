distrib:
	debuild -i -us -uc -b

distrib-up:
	debuild -nc -i -us -uc -b

distclean:
	test -n "$(DO_NOT_CLEAN_SOURCES)" || fakeroot sh -c 'DO_NOT_CLEAN_SOURCES=1 debian/rules clean'