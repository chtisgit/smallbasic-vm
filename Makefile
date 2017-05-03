
CXXFLAGS := $(CXXFLAGS) -I ./vm

all: assembler/sbas vm/sbvm
	@printf '\nSuccess!\n'

assembler/sbas: assembler/sbas.cpp
	cd assembler && $(MAKE) all

vm/sbvm: vm/main.cpp
	cd vm && $(MAKE) all

clean:
	cd assembler && $(MAKE) clean
	cd vm && $(MAKE) clean

.PHONY: clean
