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
		assert(ip >= 0 && ip < file.size());
	}

	auto jump(int a) -> void
	{
		ip = a;
		assert(ip >= 0 && ip < file.size());
	}

	auto conditional_reljump(int then_addr, int else_addr) -> void
	{
		if(cf){
			advance(then_addr);
		}else{
			advance(else_addr);
		}
		cf = false;
	}

	auto call(int a) -> void
	{
		callstack.push_back(ip);
		jump(a);
	}

	auto ret() -> bool
	{
		if(callstack.empty())
			return false;
		jump(callstack.back());
		callstack.pop_back();
		return true;
	}

	auto decode() -> uint8_t
	{
		uint8_t op = file.code()[ip];
		return op;
	}

	auto decode2() -> int16_t
	{
		const auto *c = &file.code()[ip];
		return (int32_t(c[3])) | (int32_t(c[4]) << 8);
	}

	auto decode1() -> int16_t
	{
		const auto *c = &file.code()[ip];
		return int32_t(c[1]) | (int32_t(c[2]) << 8);
	
	}

	auto decode_imm32() -> int32_t
	{
		const auto *c = &file.code()[ip];
		return uint32_t(c[1]) | (uint32_t(c[2]) << 8)
			| (uint32_t(c[3]) << 16) | (uint32_t(c[4]) << 24);
	}

};

