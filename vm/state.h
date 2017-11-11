#pragma once

#include "codefile.h"

#include <vector>

#include <cassert>
#include <cstdint>
#include <map>
#include "value.h"

class VMState{
	std::vector<int> callstack;

	int ip = 0; // instruction pointer
	bool cf = false; // condition flag
		
public:
	const CodeFile& file;
	
	std::map<int, Value> registers;
	std::vector<Value> stack;

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
		assert(ip >= 0 && ip < int(file.size()));
	}

	auto jump(int a) -> void
	{
		ip = a;
		assert(ip >= 0 && ip < int(file.size()));
	}
	
	auto set_cond(bool c) -> void
	{
		cf = c;
	}

	auto conditional_reljump(int addr, int n) -> void
	{
		if((cf && n) || (!cf && !n)){
			advance(5);
		}else{
			jump(addr); //~reljump lul
		}
		cf = false;
	}

	auto call(int a) -> void
	{
		callstack.push_back(ip + 5);
		jump(a);
	}

	auto ret() -> bool
	{
		if(callstack.empty()){	
			std::cout << ":)" <<std::endl;
			return false;
		}
		jump(callstack.back());
		callstack.pop_back();
		return true;
	}

	auto decode() -> uint8_t
	{
		uint8_t op = file.code()[ip];
		return op;
	}	
	
	auto src1() -> int8_t
	{
		const auto *c = &file.code()[ip];
		return int32_t(c[2]);
	
	}

	auto src2() -> int8_t
	{
		const auto *c = &file.code()[ip];
		return (int32_t(c[3]));
	
	}

	auto dst() -> int8_t
	{
		const auto *c = &file.code()[ip];
		return (int32_t(c[1]));
	
	}

	auto imm16(int off) -> int16_t
	{
		const auto *c = &file.code()[ip];
		return (int32_t(c[off+1])) | (int32_t(c[off+2]) << 8);
	}

	auto imm32() -> int32_t
	{
		const auto *c = &file.code()[ip];
		return uint32_t(c[1]) | (uint32_t(c[2]) << 8)
			| (uint32_t(c[3]) << 16) | (uint32_t(c[4]) << 24);
	}

};

