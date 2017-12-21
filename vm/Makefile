
CXX ?= g++
RM ?= rm
SHELL ?= /bin/sh

CXXFLAGS := $(CXXFLAGS) -O3 -std=c++11
DEBUGFLAGS := -DDEBUG -g -Wall
RELEASEFLAGS := -DNDEBUG -s

LIBS = -ldl

all: release debug

release: clean clean_release sbvm

debug: clean clean_debug sbvm_debug

sbvm: main.cpp codefile.h state.h value.h object.h
	$(CXX) $(LFLAGS) $(CXXFLAGS) $(RELEASEFLAGS) -o $@ main.cpp $(LIBS)

sbvm_debug: main.cpp codefile.h state.h value.h object.h
	$(CXX) $(LFLAGS) $(CXXFLAGS) $(DEBUGFLAGS) -o $@ main.cpp $(LIBS)

clean: clean_debug clean_release
	$(RM) *.o

clean_debug:
	$(RM) sbvm_debug

clean_release:
	$(RM) sbvm

.PHONY: all clean clean_debug clean_release release debug
