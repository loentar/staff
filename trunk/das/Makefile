#
#
# == defines ==============================================================

MAKECMDGOALS ?= make

include ../Makefile.env

ifeq ($(CROSSCOMPILE),0)
# uniq dependencies
UNIQ_DEPS := | sed 's/^\(.*\)\.\(.*\)$$/\2 \1/g' | uniq -f 1 | sed 's/^\(.*\) \(.*\)$$/\2.\1/g'
endif

MAKEFILES_DEP := $(shell find . -type f |grep -v '/samples/'|grep '/[^/]*/[^/]*$$' |egrep '(.*/Makefile\.(host)?dep$$)' | sed 's/^.\///g' $(UNIQ_DEPS))
MAKE_ORDER_DEPS := $(subst /Makefile,,$(MAKEFILES_DEP))
ifeq ($(CROSSCOMPILE),0)
MAKE_ORDER_DEPS := $(patsubst %.hostdep,%.dep,$(MAKE_ORDER_DEPS))
endif

ifeq "0" "$(PARALLEL)"
.NOTPARALLEL: $(MAKE_ORDER_DEPS)
endif

#.PHONY: $(MAKECMDGOALS)

#
#
# ==  GOALS  ==============================================================

$(MAKECMDGOALS): $(MAKE_ORDER_DEPS)

# must be declared under main goal
ifeq ($(CROSSCOMPILE),0)
# include dep files, replacing ".hostdep" -> ".dep"
$(eval $(subst ^,$(newline),$(shell cat $(MAKEFILES_DEP) | sed 's/\.hostdep/.dep/g' | tr '\n' '^')))
else
# include dependencies, taking care about host and cross building
include $(MAKEFILES_DEP)
endif

%.dep:
ifeq "0" "$(PARALLEL)"
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
endif
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)

ifeq ($(CROSSCOMPILE),1)
%.hostdep:
ifeq "0" "$(PARALLEL)"
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %.hostdep,%,$@)\033[0m"
endif
	$(MAKE) -C $(patsubst %.hostdep,%,$@) $(MAKECMDGOALS) $(HOSTMAKEFLAGS)
endif


ifeq ($(MAKECMDGOALS),distrib)
DISTRIBDIR = $(TOPDIR)/distrib/
include Makefile.distrib
endif
