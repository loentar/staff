#
#
# == defines ==============================================================

MAKECMDGOALS ?= make

include ../Makefile.env
ifneq ($(MAKECMDGOALS),test)
  EXCLUDE_TESTS := -a \! -path './tests/*'
endif
MAKEFILES_DEP = $(shell find . -type f -name Makefile.dep -a \! -path './samples/*' $(EXCLUDE_TESTS) | sed 's/^.\///g')

MAKE_ORDER_DEPS = $(patsubst %/Makefile.dep,%.dep,$(MAKEFILES_DEP))
ifeq "0" "$(PARALLEL)"
.NOTPARALLEL: $(MAKE_ORDER_DEPS)
endif

#.PHONY: $(MAKECMDGOALS)

#
#
# ==  GOALS  ==============================================================

$(MAKECMDGOALS): upgrade $(MAKE_ORDER_DEPS)

# must be declared under main goal
include $(MAKEFILES_DEP)

%.dep:
ifeq "0" "$(PARALLEL)"
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
endif
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)

upgrade:
	test -z "$(STAFF_HOME)" -o -L "$(STAFF_HOME)/include/staff" || rm -Rf "$(STAFF_HOME)/include/staff"
