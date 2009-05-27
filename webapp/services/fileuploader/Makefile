include ../../Makefile.env

TARGET = fileuploader
TARGETNAME = lib$(TARGET).so

CXXFLAGS += -fPIC -fexceptions -D_REENTRANT -Wall
LDFLAGS += -fPIC -fexceptions -Wl,-rpath,$(AXIS2C_LIBDIR)

VPATH = $(SRCDIR)

INSTALLDIR = $(STAFF_HOME)/components/

CXXFLAGS += -Wno-deprecated -I$(STAFF_DEPLOY_HOME)/include/
LDFLAGS += -L$(STAFF_DEPLOY_HOME)/lib
LDFLAGS += -lstaffcommon -lstaffcomponent -lstaffsecurity -lrise -lpthread
LDFLAGS += -shared -Wl,-soname,$(TARGETNAME)

COMPONENT_SERVICES := $(patsubst $(SRCDIR)%Impl.h, %, $(filter-out $(SRCDIR)ComponentImpl.h, $(wildcard $(SRCDIR)*Impl.h)))
COMPONENT_SERVICES_HEADERS := $(patsubst %, $(SRCDIR)%.h, $(COMPONENT_SERVICES))

#EXTERN_SERVICES := $(patsubst $(SRCDIR)%.h, %, $(filter-out %Wrapper.h %Context.h %Impl.h %Proxy.h %ServiceLocator.h $(COMPONENT_SERVICES_HEADERS), $(wildcard $(SRCDIR)*.h)))
#EXTERN_SERVICES_HEADERS := $(patsubst %, $(SRCDIR)%.h, $(EXTERN_SERVICES))

COMPONENT_SERVICES_CODEGEN := $(SRCDIR)ComponentImpl.h $(SRCDIR)ComponentImpl.cpp \
    $(patsubst %.h,%Context.h,$(COMPONENT_SERVICES_HEADERS)) $(patsubst %.h,%Context.cpp,$(COMPONENT_SERVICES_HEADERS)) \
    $(patsubst %.h,%Wrapper.h,$(COMPONENT_SERVICES_HEADERS)) $(patsubst %.h,%Wrapper.cpp,$(COMPONENT_SERVICES_HEADERS))

ifneq ("$(EXTERN_SERVICES)","")
EXTERN_SERVICES_CODEGEN := $(SRCDIR)ServiceLocator.h $(SRCDIR)ServiceLocator.cpp \
    $(patsubst %.h,%Proxy.h,$(EXTERN_SERVICES_HEADERS)) $(patsubst %.h,%Proxy.cpp,$(EXTERN_SERVICES_HEADERS))
endif

CODEGEN_SOURCES := $(filter %.cpp,$(COMPONENT_SERVICES_CODEGEN) $(EXTERN_SERVICES_CODEGEN))

SOURCES = $(filter-out $(CODEGEN_SOURCES) ,$(wildcard $(SRCDIR)*.cpp)) $(CODEGEN_SOURCES)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" codegen $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

# compile
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# == codegen ===========================================
codegen: $(COMPONENT_SERVICES_CODEGEN) $(EXTERN_SERVICES_CODEGEN)

# генерация оберток сервисов компонента
$(SRCDIR)%Context.h $(SRCDIR)%Context.cpp $(SRCDIR)%Wrapper.h $(SRCDIR)%Wrapper.cpp:: $(SRCDIR)%.h
	cd $(SRCDIR) && $(STAFF_CODEGEN) -tcomponent $(patsubst $(SRCDIR)%,%,$(COMPONENT_SERVICES_HEADERS))

$(SRCDIR)ComponentImpl.h $(SRCDIR)ComponentImpl.cpp:: $(COMPONENT_SERVICES_HEADERS)
	cd $(SRCDIR) && $(STAFF_CODEGEN) -tcomponent $(patsubst $(SRCDIR)%,%,$(COMPONENT_SERVICES_HEADERS))

# генерация прокси внешних сервисов
ifeq ("$(EXTERN_SERVICES)","")
$(SRCDIR)ServiceLocator.h $(SRCDIR)ServiceLocator.cpp:;
#	cd $(SRCDIR) && $(STAFF_CODEGEN) -tservicelocator $(patsubst $(SRCDIR)%,%,$(EXTERN_SERVICES_HEADERS))
else
$(SRCDIR)%Proxy.h $(SRCDIR)%Proxy.cpp:: $(SRCDIR)%.h
	cd $(SRCDIR) && $(STAFF_CODEGEN) -tservicelocator $(patsubst $(SRCDIR)%,%,$(EXTERN_SERVICES_HEADERS))

$(SRCDIR)ServiceLocator.h $(SRCDIR)ServiceLocator.cpp:: $(EXTERN_SERVICES_HEADERS)
	cd $(SRCDIR) && $(STAFF_CODEGEN) -tservicelocator $(patsubst $(SRCDIR)%,%,$(EXTERN_SERVICES_HEADERS))
endif

# == deploy  ========================================
deploy: "$(DEPLOYDIR)usr/local/staff/components/$(TARGET)/" "$(DEPLOYDIR)usr/local/staff/components/$(TARGET)/config/"
	cp -f $(OUTDIR)$(TARGETNAME) $<
	cp -f config/*.xml $(DEPLOYDIR)usr/local/staff/components/$(TARGET)/config/

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(TARGET)/" "$(INSTALLDIR)$(TARGET)/config/"
	cp -f $(OUTDIR)$(TARGETNAME) "$(INSTALLDIR)$(TARGET)/"
	cp -f config/*.xml $(INSTALLDIR)$(TARGET)/config/
	chmod 777 $(INSTALLDIR)$(TARGET)/config/

# == uninstall ======================================
uninstall:
	rm -f $(INSTALLDIR)components/$(TARGET)/$(TARGETNAME)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)
	rm -f $(COMPONENT_SERVICES_CODEGEN) $(EXTERN_SERVICES_CODEGEN)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
