#
#
# == defines ==============================================================

MAKECMDGOALS ?= test

LOGFILE := $(MAKECMDGOALS).log

ECHO := echo$(shell test -z "$$(echo -e)" && echo ' -e ')

MAKEFILE_PATHS := $(patsubst ./%/Makefile,%/_,$(shell find . -mindepth 2 -maxdepth 3 -type f -name Makefile))

#
#
# ==  GOALS  ==============================================================

$(MAKECMDGOALS): test_init $(MAKEFILE_PATHS)

%/_:
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %/_,%,$@)\033[0m"
	@$(MAKE) -sC $(patsubst %/_,%,$@) $(MAKECMDGOALS) >>$(LOGFILE)
	@$(ECHO) "\033[1m$(MAKECMDGOALS): $(patsubst %/_,%,$@): OK\033[0m"

test_init:
	@echo -n >$(LOGFILE)