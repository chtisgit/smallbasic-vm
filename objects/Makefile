
RM ?= rm
CXX ?= g++
CXXFLAGS := $(CXXFLAGS) -std=c++11 -fPIC -I../vm

OBJS = object0.so object1.so object2.so

all: $(OBJS)

%.so: %.cpp
	$(CXX) -shared $(CXXFLAGS) -o $@ $<

clean:
	$(RM) *.so

.PHONY: all clean
