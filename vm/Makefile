
CXX ?= g++
RM ?= rm
SHELL ?= /bin/sh

CXXFLAGS := $(CXXFLAGS) -g -O3 -Wall -std=c++11
OBJS = main.o

all: sbvm

sbvm: $(OBJS)
	$(CXX) $(LFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) *.o sbvm

.PHONY: all clean
