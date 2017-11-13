
CXX ?= g++
RM ?= rm
SHELL ?= /bin/sh

CXXFLAGS := $(CXXFLAGS) -g -O0 -Wall -std=c++11 
OBJS = sbas.o sb_output.o sb_opcodes.o

all: sbas

sbas: $(OBJS)
	$(CXX) $(LFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) *.o sbas

.PHONY: all clean
