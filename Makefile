#
#
# == defines ==============================================================

MAKECMDGOALS ?= make

ECHO := echo$(shell test -z "$$(echo -e)" && echo ' -e ')

MAKEFILES_DEP = $(wildcard */Makefile.dep)
MAKE_ORDER_DEPS = $(patsubst %/Makefile.dep,%.dep,$(MAKEFILES_DEP))

ifeq "" "$(PARALLEL)"
.NOTPARALLEL: $(MAKE_ORDER_DEPS)
endif

#.PHONY: $(MAKECMDGOALS)

#
#
# ==  GOALS  ==============================================================

ifeq ($(MAKECMDGOALS),distclean)
$(MAKECMDGOALS):
	$(MAKE) clean
	$(MAKE) -C staff/samples clean
	find das/samples -name Makefile -exec bash -c "echo {} | sed 's/[^\/]*$$//g' | xargs $(MAKE) clean -C" \;
	find . -type d -a \( -name deploy -o -name out -o -name obj \) | xargs rm -Rf
else
ifeq ($(MAKECMDGOALS),test)
$(MAKECMDGOALS):
	$(MAKE) -C rise/test/rise_test test
	$(MAKE) -C staff/tests test
else
$(MAKECMDGOALS): $(MAKE_ORDER_DEPS)
endif
endif

# must be declared under main goal
include $(MAKEFILES_DEP)

%.dep:
ifeq "" "$(PARALLEL)"
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
endif
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)
