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

ifeq ($(MAKECMDGOALS),regenclients)
include Makefile.env
$(MAKECMDGOALS):
	$(STAFF_CODEGEN) -tjsclient -owebclient/webapp/clients\
 ../staff/components/Login/src/Login.h\
 ../staff/components/admin/AccountAdmin/src/AccountAdmin.h\
 ../staff/components/admin/ObjectAdmin/src/ObjectAdmin.h\
 services/FileUploader/src/FileUploader.h\
 services/ProfileAdmin/src/ProfileAdmin.h\
 services/WidgetManager/src/WidgetManager.h
else
$(MAKECMDGOALS): $(MAKE_ORDER_DEPS)
endif

# must be declared under main goal
include $(MAKEFILES_DEP)

%.dep:
	@/bin/echo -e "\n\033[1m$(MAKECMDGOALS): $(patsubst %.dep,%,$@)\033[0m"
	$(MAKE) -C $(patsubst %.dep,%,$@) $(MAKECMDGOALS)

PROJECT=webapp
VERSION=1.1.0a

include ../Makefile.distrib
