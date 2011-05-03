#
#
# == defines ==============================================================

ifeq "" "$(MAKECMDGOALS)"
  MAKECMDGOALS = make
endif

ECHO := echo$(shell test $$(readlink /bin/sh) = "dash" || echo ' -e ')
ifneq ($(MAKECMDGOALS),test)
  EXCLUDE_TESTS := -a \! -path './tests/*'
endif
MAKEFILES_DEP = $(shell find . -type f -name Makefile.dep -a \! -path './samples/*' $(EXCLUDE_TESTS) | sed 's/^.\///g')

MAKE_ORDER_DEPS = $(patsubst %/Makefile.dep,%.dep,$(MAKEFILES_DEP))
.NOTPARALLEL: $(MAKE_ORDER_DEPS)

#.PHONY: $(MAKECMDGOALS)

#
#
# ==  GOALS  ==============================================================

$(MAKECMDGOALS): $(MAKE_ORDER_DEPS)

# must be declared under main goal
include $(MAKEFILES_DEP)

%.dep:
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)

PROJECT=staff
VERSION=2.0.0-tp5

ifeq ($(MAKECMDGOALS),distrib)
include ../Makefile.distrib
endif
