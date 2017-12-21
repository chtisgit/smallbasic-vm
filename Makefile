
CXXFLAGS := $(CXXFLAGS) -I ./vm

all: assembler/sbas vm/sbvm objects
	@printf '\nSuccess!\n'

assembler/sbas: assembler/sbas.cpp
	cd assembler && $(MAKE) all

vm/sbvm: vm/main.cpp
	cd vm && $(MAKE) all

objects:
	cd objects && $(MAKE) all

clean:
	cd assembler && $(MAKE) clean
	cd vm && $(MAKE) clean
	cd objects && $(MAKE) clean

.PHONY: clean all objects
