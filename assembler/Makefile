
CXX ?= g++
RM ?= rm
SHELL ?= /bin/sh

CXXFLAGS := $(CXXFLAGS) -g -O3 -Wall -std=c++11 
OBJS = sbas.o

all: sbas

sbas: $(OBJS)
	$(CXX) $(LFLAGS) -o $@ $^

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) *.o sbas

.PHONY: all clean
