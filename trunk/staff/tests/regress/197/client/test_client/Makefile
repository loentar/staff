include ../../../../../Makefile.env

TARGET = simple_client
TARGETNAME = $(TARGET)

HEADERS := $(filter-out %ServiceFactory.h %Proxy.h,$(wildcard $(SRCDIR)*.h))

CXXFLAGS += -I$(DEPLOYDIR)include
LDFLAGS += -L$(DEPLOYDIR)lib -lstaffutils -lstaffcommon -lstaffclient

VPATH = $(subst $(empty) $(empty),:,$(SRCDIR))

CODEGENH := $(patsubst %.h,%Proxy.h,$(HEADERS))
CODEGENCPP := $(patsubst %.h,%Proxy.cpp,$(HEADERS))
CODEGEN := $(CODEGENH) $(CODEGENCPP)

SOURCES = $(CODEGENCPP) $(filter-out %Proxy.cpp,$(wildcard $(SRCDIR)*.cpp))
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.SECONDARY: $(CODEGEN)

# == make ===========================================
make: check $(CODEGEN) "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME)

test: make
	$(OUTDIR)$(TARGETNAME)

# link
$(OUTDIR)$(TARGETNAME): $(CODEGEN) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME) $(LDFLAGS)

# compile
$(OBJDIR)%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SRCDIR)%Proxy.h $(SRCDIR)%Proxy.cpp:: $(SRCDIR)%.h
	[ "$@" -nt "$^" ] || (cd src && $(STAFF_CODEGEN) -e $(patsubst $(SRCDIR)%,%,$(HEADERS)) -tclient -n$(TARGET))
	sed -i '/\/\/ synchronous call/,/rdoResult = /d;s/return rdoResult.*/rdoRequest.SetOwner(false);\n  return rdoRequest;/' $(SRCDIR)TestQualifiedProxy.cpp $(SRCDIR)TestUnqualifiedProxy.cpp

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)
	rm -f $(CODEGEN)
	-rmdir -p obj out 2>/dev/null

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@
