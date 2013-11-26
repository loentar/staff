#
#
# == defines ==============================================================

MAKECMDGOALS ?= make

include ../Makefile.env
ifneq ($(MAKECMDGOALS),test)
  EXCLUDE_TESTS := |grep -v 'tests'
endif
ifneq ($(WITHOUT_SECURITY),)
  EXCLUDE_SECURITY := | sed '/\/components\/\(Login\|admin\)\/\|\/axis2\/staff_security_module\/\|\/core\/security\/\|\/core\/sqlite3\//d'
endif
ifeq ($(CROSSCOMPILE),0)
# uniq dependencies
UNIQ_DEPS := | sed 's/^\(.*\)\.\(.*\)$$/\2 \1/g' | uniq -f 1 | sed 's/^\(.*\) \(.*\)$$/\2.\1/g'
endif

MAKEFILES_DEP := $(shell find . -type f | grep -v '/samples/'|grep '/[^/]*/[^/]*$$' |egrep '(.*/Makefile\.(host)?dep$$)'$(EXCLUDE_TESTS)$(EXCLUDE_SECURITY) | sed 's/^.\///g' $(UNIQ_DEPS))
MAKE_ORDER_DEPS := $(subst /Makefile,,$(MAKEFILES_DEP))
ifeq ($(CROSSCOMPILE),0)
MAKE_ORDER_DEPS := $(patsubst %.hostdep,%.dep,$(MAKE_ORDER_DEPS))
else
ifeq ($(MAKECMDGOALS),install)
MAKE_ORDER_DEPS := $(filter %.dep,$(MAKE_ORDER_DEPS))
endif
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
$(filter %.dep,$(MAKE_ORDER_DEPS)): $(filter %.hostdep,$(MAKE_ORDER_DEPS))
# include dependencies, taking care about host and cross building
include $(MAKEFILES_DEP)
endif

%.dep:
ifeq "0" "$(PARALLEL)"
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
endif
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)

ifeq ($(CROSSCOMPILE),1)
ifneq ($(MAKECMDGOALS),install)
%.hostdep:
ifeq "0" "$(PARALLEL)"
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %.hostdep,%,$@)\033[0m"
endif
	$(MAKE) -C $(patsubst %.hostdep,%,$@) $(MAKECMDGOALS) $(HOSTMAKEFLAGS)
endif
endif

