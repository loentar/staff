#
#
# == defines ==============================================================

ifeq "" "$(MAKECMDGOALS)"
  MAKECMDGOALS = make
endif
ifeq "all" "$(MAKECMDGOALS)"
  MAKECMDGOALS = make
endif

ECHO := echo$(shell test -z "$$(echo -e)" && echo ' -e ')

MAKEFILES_DEP = $(shell find . -type f -name Makefile.dep | sed 's/^.\///g')
MAKE_ORDER_DEPS = $(patsubst %/Makefile.dep,%.dep,$(MAKEFILES_DEP))
.NOTPARALLEL: $(MAKE_ORDER_DEPS)

#.PHONY: $(MAKECMDGOALS)

#
#
# ==  GOALS  ==============================================================


ifneq ($(MAKECMDGOALS),distclean)
$(MAKECMDGOALS): $(MAKE_ORDER_DEPS)
all: make
else
$(MAKECMDGOALS):
	$(MAKE) clean
	find . -type d -a \( -name deploy -o -name out -o -name obj \) | xargs rm -Rfv
endif

# must be declared under main goal
include $(MAKEFILES_DEP)

%.dep:
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)

PROJECT=staff-das
VERSION=1.0.0

ifeq ($(MAKECMDGOALS),distrib)
DISTRIBDIR = $(TOPDIR)/distrib/
include Makefile.distrib
endif
