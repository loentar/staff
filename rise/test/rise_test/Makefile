TARGET = rise_test
PROJECT = $(TARGET)

SRCDIR = src/

ifeq ("$(PLATFORM)","")
  PLATFORM := $(shell uname -m)
endif

OBJDIR = obj/$(PLATFORM)/
OUTDIR = out/$(PLATFORM)/

CXXFLAGS += -D_REENTRANT -Wall -rdynamic -Wno-deprecated -fexceptions -fPIC -I../..
LIBS = -lpthread -lstdc++ -lrise -L../../$(OUTDIR)

ifndef ("$(DEBUG)","")
  CXXFLAGS += -DDEBUG -g -O0
  LDFLAGS += -g -O0
endif

VPATH = $(SRCDIR)

SOURCES := $(wildcard $(SRCDIR)*.cpp)
OBJECTS := $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(SOURCES))

# == make ===========================================
make: "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGET)
	make -C ../myplugin

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
	rm -Rf $(OBJDIR) $(OUTDIR)

# == mkdir ==========================================
"%/":
	@mkdir -p $@ && chmod g+w $@
