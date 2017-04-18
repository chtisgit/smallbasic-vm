#include <iostream>
#include <vector>

#include <cassert>
#include <cstdint>

#include "codefile.h"

class VMState{
	CodeFile& file;
	int commands;

	size_t ip = 0; // instruction pointer

public:

	VMState(CodeFile& f, int cm)
		: file(f), commands(cm)
	{
	}

	VMState(const VMState&) = delete;
	VMState(VMState&&) = delete;

	~VMState()
	{
	}

	auto advance() -> uint8_t
	{
		assert(ip < file.size());
		auto op = file.code()[ip++];
		assert(op < commands);
		return op;
	}

	

};

