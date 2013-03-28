#
#
# == defines ==============================================================

MAKECMDGOALS ?= make

MAKEFILES_DEP = $(wildcard */Makefile.dep)
MAKE_ORDER_DEPS = $(patsubst %/Makefile.dep,%.dep,$(MAKEFILES_DEP))

include Makefile.env

ifeq "0" "$(PARALLEL)"
.NOTPARALLEL: $(MAKE_ORDER_DEPS)
endif

.PHONY: $(MAKECMDGOALS)

#
#
# ==  GOALS  ==============================================================

ifeq ($(MAKECMDGOALS),distclean)
$(MAKECMDGOALS):
	-$(MAKE) -Cdistrib $(MAKECMDGOALS)
	$(MAKE) clean
	$(MAKE) -C staff/samples clean
	find das/samples -name Makefile | sed 's/[^\/]*$$//g' | xargs -L 1 $(MAKE) clean -C
	find . -type d -a \( -name deploy -o -name out -o -name obj \) | xargs rm -Rf
else
ifeq ($(MAKECMDGOALS),test)
$(MAKECMDGOALS):
	$(MAKE) -C staff/tests test
else
ifeq ($(MAKECMDGOALS),distrib)
$(MAKECMDGOALS):
	$(MAKE) -Cdistrib $(MAKECMDGOALS)
else
$(MAKECMDGOALS): $(MAKE_ORDER_DEPS)
endif
endif
endif

# must be declared under main goal
include $(MAKEFILES_DEP)

%.dep:
ifeq "0" "$(PARALLEL)"
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
endif
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)
