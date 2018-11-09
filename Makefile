CC := g++
SRCDIR := .
BINDIR := bin
BUILDDIR := build
TARGET := aes-finder
UNAME := $(shell uname)

# Debug Flags
DEBUGFLAGS := -g3 -O0 -Wall -fomit-frame-pointer -march=native
RTCHECKS := -fmudflap -fstack-check -gnato

# Production Flags
PRODFLAGS := -Wall -O3 -fomit-frame-pointer -march=native

ifeq ($(UNAME), Linux)
# Linux Flags
CFLAGS := -std=c++11 -Iinclude/ $(DEBUGFLAGS)
LINK := -lpthread -lkqueue $(DEBUGFLAGS)
else
# OSX / BSD Flags
CFLAGS := -std=c++11 -stdlib=libc++ -Iinclude/ $(DEBUGFLAGS)
LINK := -stdlib=libc++ $(DEBUGFLAGS)
endif
 
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

$(TARGET): $(OBJECTS)
	@echo " Linking..."; $(CC) $(LINK) $^ -o $(BINDIR)/$(TARGET)
 
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CC $<"; $(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo " Cleaning..."; rm -r $(BUILDDIR) $(BINDIR)/$(TARGET)*
 
.PHONY: clean