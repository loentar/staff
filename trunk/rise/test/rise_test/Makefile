TOPDIR ?= $(shell pwd | sed 's/$$/\//g' | sed 's/^\(.*\/rise\/\).*/\1/g' | grep \/rise\/)
include $(TOPDIR)/../Makefile.env

TARGET = rise_test
PROJECT = $(TARGET)

SRCDIR = src/

ifeq ("$(PLATFORM)","")
  PLATFORM := $(shell uname -m)
endif

OBJDIR = obj/$(PLATFORM)/
OUTDIR = out/$(PLATFORM)/

DEPLOYDIR = ../../deploy/$(PLATFORM)

CXXFLAGS += -DRISE_TEST -rdynamic -I$(DEPLOYDIR)/usr/include
LIBS = -L$(DEPLOYDIR)/usr/lib/ -lpthread -lstdc++ -lrise

ifndef ("$(DEBUG)","")
  CXXFLAGS += -DDEBUG -g -O0
  LDFLAGS += -g -O0
endif

VPATH = $(SRCDIR)

SOURCES := $(wildcard $(SRCDIR)*.cpp)
OBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(SOURCES))

# == make ===========================================
make: "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGET)
	$(MAKE) -C ../myplugin

# == test ===========================================
test: make
	unset RISE_LOG_LEVEL; LD_LIBRARY_PATH=../myplugin/$(OUTDIR):../../$(OUTDIR) $(OUTDIR)$(TARGET)

# == compile ========================================
$(OBJDIR)%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(OUTDIR)$(TARGET): $(OBJECTS)
	$(CXX) $(LIBS) $(CXXFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGET)

# == clean ==========================================
clean:
	$(MAKE) -C ../myplugin clean
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@mkdir -p $@ && chmod g+w $@
