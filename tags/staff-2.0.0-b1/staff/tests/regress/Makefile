#
#
# == defines ==============================================================

MAKECMDGOALS ?= test

LOGFILE := $(MAKECMDGOALS).log

include ../../../Makefile.env

MAKEFILE_PATHS := $(patsubst ./%/Makefile,%/_,$(shell find . -mindepth 2 -maxdepth 3 -type f -name Makefile | sort -nr))

#
#
# ==  GOALS  ==============================================================

$(MAKECMDGOALS): test_init $(MAKEFILE_PATHS)

%/_:
	@$(ECHO) "\n\033[1m$(MAKECMDGOALS): $(patsubst %/_,%,$@)\033[0m" | tee -a $(LOGFILE)
	@$(MAKE) -sC $(patsubst %/_,%,$@) $(MAKECMDGOALS) >>$(LOGFILE)
	@$(ECHO) "\033[1m$(MAKECMDGOALS): $(patsubst %/_,%,$@): OK\033[0m" | tee -a $(LOGFILE)

test_init:
	@echo -n >$(LOGFILE)
