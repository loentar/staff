include ../../Makefile.env

TARGET = calculator_service
TARGETNAME = $(TARGET)

HEADERS := $(filter-out %Wrapper.h %Impl.h,$(wildcard $(SRCDIR)*.h))

CXXFLAGS += -c -Wno-deprecated -fexceptions -I$(DEPLOYDIR)include -I$(STAFF_HOME)/include/
LDFLAGS += -fexceptions -L$(DEPLOYDIR)lib -L$(STAFF_HOME)/lib
LDFLAGS += -lstaffcommon -lstaffservice -lstaffsecurity -lrise -laxis2_axiom -laxutil -laxis2_engine $(LDAXIS2LIBS)

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

CODEGENH := $(patsubst %.h,%Wrapper.h,$(HEADERS))
CODEGENCPP := $(patsubst %.h,%Wrapper.cpp,$(HEADERS))
CODEGEN := $(CODEGENH) $(CODEGENCPP)

WSDLS := $(patsubst %.h, %.wsdl, $(HEADERS))

SOURCES = $(CODEGENCPP) $(filter-out %Wrapper.cpp,$(wildcard $(SRCDIR)*.cpp))
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.SECONDARY: $(CODEGEN)

# == make ===========================================
make: check "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME) deploy

# link
$(OUTDIR)$(TARGETNAME): $(CODEGEN) $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

$(OBJECTS): $(CODEGEN)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(CODEGEN): $(HEADERS) $(WSDLS)
	cd src; $(STAFF_CODEGEN) -u $(patsubst $(SRCDIR)%,%,$(HEADERS)) -tservice -n$(TARGET)

%.wsdl: %.h
	$(STAFF_CODEGEN) -u -twsdl $(patsubst $(SRCDIR)%,%,$<) -i$(SRCDIR) -o$(OUTDIR)

# == deploy  ========================================
deploy: "$(DEPLOYDIR)$(SAMPLESDIR)$(BINDIR)" $(OUTDIR)$(TARGETNAME)
	cp -f $(OUTDIR)$(TARGETNAME) $(DEPLOYDIR)$(SAMPLESDIR)$(BINDIR)$(TARGETNAME)

# == distrib =========================================
distrib:;

# == install ========================================
install: check "$(INSTALLDIR)$(SAMPLESDIR)$(BINDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(SAMPLESDIR)$(BINDIR)$(TARGETNAME)

# == uninstall ======================================
uninstall: check
	rm -f $(INSTALLDIR)$(SAMPLESDIR)$(BINDIR)$(TARGETNAME)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)
	rm -f $(CODEGEN)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
