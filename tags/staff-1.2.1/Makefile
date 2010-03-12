#
#
# == defines ==============================================================

ifeq "" "$(MAKECMDGOALS)"
  MAKECMDGOALS = make
endif

MAKEFILES_DEP = $(wildcard */Makefile.dep)
MAKE_ORDER_DEPS = $(patsubst %/Makefile.dep,%.dep,$(MAKEFILES_DEP))

.NOTPARALLEL: $(MAKE_ORDER_DEPS)

#.PHONY: $(MAKECMDGOALS)

#
#
# ==  GOALS  ==============================================================

ifneq ($(MAKECMDGOALS),distclean)
$(MAKECMDGOALS): $(MAKE_ORDER_DEPS)
else
$(MAKECMDGOALS):
	find -type d -a \( -name deploy -o -name out -o -name obj \) | xargs rm -Rfv
endif

# must be declared under main goal
include $(MAKEFILES_DEP)

%.dep:
	@/bin/echo -e "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)
