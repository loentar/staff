#
#
# == defines ==============================================================

ifeq "" "$(MAKECMDGOALS)"
  MAKECMDGOALS = make
endif

MAKEFILES_DEP = $(shell find -type f -name Makefile.dep | sed 's/^.\///g')
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
	@/bin/echo -e "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
#	@echo "\n\n$(patsubst %.dep,%,$@) => $(patsubst %.dep,%,$^)"
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)

PROJECT=webapp
VERSION=1.1.0a

include ../Makefile.distrib
