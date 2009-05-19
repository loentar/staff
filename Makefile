#
#
# == defines ==============================================================

ifeq "" "$(MAKECMDGOALS)"
  MAKECMDGOALS = make
endif

MAKEFILES_DEP = $(wildcard */Makefile.dep)
MAKE_ORDER_DEPS = $(patsubst %/Makefile.dep,%.dep,$(MAKEFILES_DEP))

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
