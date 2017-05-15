#pragma once

#include "codefile.h"

#include <vector>

#include <cassert>
#include <cstdint>

template<unsigned commands>
class VMState{
	CodeFile& file;

	int ip = 0; // instruction pointer

public:

	VMState(CodeFile& f)
		: file(f)
	{
	}

	VMState(const VMState&) = delete;
	VMState(VMState&&) = delete;

	~VMState()
	{
	}

	auto advance(int a) -> void
	{
		ip += a;
		assert(ip >= 0 && ip < file.size());
	}

	auto jump(int a) -> void
	{
		ip = a;
		assert(ip >= 0 && ip < file.size());
	}


	auto decode() -> uint8_t
	{
		uint8_t op = file.code()[ip];
		assert(unsigned(op) < commands);
		return op;
	}

	auto decode_imm32() -> int32_t
	{
		const auto *c = &file.code()[ip];
		return uint32_t(c[1]) | (uint32_t(c[2]) << 8)
			| (uint32_t(c[3]) << 16) | (uint32_t(c[4]) << 24);
	}

};

