
TARGET = MyPlugin
PROJECT = $(TARGET)

SRCDIR = src/
TARGETLIB = lib$(TARGET).so

ifeq ("$(PLATFORM)","")
  PLATFORM := $(shell uname -m)
endif

OBJDIR = obj/$(PLATFORM)/
OUTDIR = out/$(PLATFORM)/

CXXFLAGS += -D_REENTRANT -Wall -rdynamic -Wno-deprecated -fexceptions -fPIC -I../..
LIBS = -lrise -L../../$(OUTDIR)

ifeq ($(PLATFORM),x86_64)
  CXXFLAGS += -D_M_X64
endif

ifndef ("$(DEBUG)","")
  CXXFLAGS += -DDEBUG -g -O0
  LDFLAGS += -g -O0
endif

VPATH = $(SRCDIR)

SOURCES := $(wildcard $(SRCDIR)*.cpp)
OBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(SOURCES))

# == make ===========================================
make: dirs $(OUTDIR)$(TARGETLIB)

dirs: "$(OBJDIR)" "$(OUTDIR)"

# == compile ========================================
$(OBJDIR)%.o: %.cpp
	$(CXX) -I. -c $(CXXFLAGS) $< -o $@

$(OUTDIR)$(TARGETLIB): $(OBJECTS)
	$(CXX) -shared -Wl,-soname,$(TARGETLIB) $(LIBS) $(CXXFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETLIB)

# == uninstall ======================================
uninstall:
	rm -Rf $(USRINCDIR)rise $(USRLIBDIR)$(TARGETLIB) $(USRLIBDIR)$(TARGETLIB)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@mkdir -p $@ && chmod g+w $@
